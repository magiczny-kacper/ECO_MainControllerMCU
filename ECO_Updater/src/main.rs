extern crate dotenv;

use serde::{Deserialize, Serialize};
use serde_repr::*;
use dotenv::dotenv;
use crc::{Crc, CRC_16_KERMIT};

use std::env;
use std::net::UdpSocket;
use std::fs;
use std::fs::File;
use std::io::Read;
use std::time::{Duration, Instant};

/// Frame identificator
#[repr(u8)]
#[derive(Serialize_repr, Deserialize_repr, Copy, Clone, Debug)]
enum FrameId {
    InfoFrame = 0xA0,
    ChunkFrame = 0xB5
}

#[repr(u8)]
#[derive(Serialize_repr, Deserialize_repr, Copy, Clone, Debug)]
enum BlockId {
    McuApplication = 0x00,
    SmallHmiApplication = 0x01,
    BigHmiApplication = 0x02,
    UnknownApplication
}

impl BlockId {
    fn from_str(name: &String) -> BlockId {
        match name.as_str() {
            "signed" => Self::McuApplication,
            "small" => Self::SmallHmiApplication,
            "big" => Self::BigHmiApplication,
            _ => Self::UnknownApplication
        }
    }
}

/// Frame sent to target
#[derive(Serialize, Deserialize, Debug)]
struct FrameHeader {
    /// Frame identificator
    id: FrameId,
    /// Size of data field
    size: u16,
    /// Data crc
    crc: u16,
}

/// Update info frame, should be sent at the beginning of update
#[derive(Serialize, Deserialize, Debug)]
struct InfoFrame {
    /// Updated block identificator
    block_id: BlockId,
    /// Size of file to be uploaded
    file_size: u32,
}

/// Reponse frame
#[repr(u8)]
#[derive(Serialize, Deserialize, Copy, Clone)]
enum ResponseFrame {
    RespFrameCorrupted = 0x00,
    RespOk = 0xAA,
    RespNotOk = 0x55,
}

/// Reads file and returns it as vector of bytes
fn get_file_as_byte_vec(filename: &String) -> Vec<u8> {
    let mut f = File::open(&filename).expect("no file found");
    let metadata = fs::metadata(&filename).expect("unable to read metadata");
    let mut buffer = vec![0; metadata.len() as usize];
    f.read(&mut buffer).expect("buffer overflow");

    buffer
}

fn create_socket() -> UdpSocket {
    // Socket initialization
    println!("Opening UDP socket 3400...");
    let socket = UdpSocket::bind("0.0.0.0:3400").expect("Couldn't bind to address");
    // socket.set_read_timeout(Some(Duration::new(5, 0))).expect("set_read_timeout call failed");

    socket
}

fn main() {
    // For configuration
    dotenv().ok();

    let target_ip = env::var("TARGET_IP").expect("TARGET_IP must be set");
    let update_file_name = env::var("UPDATE_FILE_MCU").expect("UPDATE_FILE_MCU must be set");
    let file_suffix = update_file_name
        .split("/").last().unwrap()
        .split(".").next().unwrap()
        .split("_").last().unwrap();
    let block_id = BlockId::from_str(&file_suffix.to_string());

    let mut rx_buf = [0; 256];
    let crc = Crc::<u16>::new(&CRC_16_KERMIT);

    println!("Preparing to send {} to target {}", update_file_name, target_ip);
    
    let socket = create_socket();

    // communication at this time
    println!("Connecting to target...");
    socket.connect(target_ip).expect("connect function failed");

    // Read file
    println!("Reading file...");
    let file_content = get_file_as_byte_vec(&update_file_name);
    println!("File size: {}b", file_content.len());

    // Start the update
    println!("Sending SWUpdate header...");
    let header = InfoFrame {
        block_id: block_id, 
        file_size: file_content.len() as u32
    };
    
    let mut header_bytes = bincode::serialize(&header).unwrap();
    
    let frame = FrameHeader {
        id: FrameId::InfoFrame,
        size: header_bytes.len() as u16,
        crc: crc.checksum(&header_bytes as &[u8])
    };
    println!("{:?}", frame);

    let mut frame = bincode::serialize(&frame).unwrap();
    frame.append(&mut header_bytes);

    println!("{:02x?}", frame);
    socket.send(&frame).expect("Failed to send header");

    match socket.recv(&mut rx_buf) {
        Ok(received) => {
            println!("Received {received} bytes {:?}", &rx_buf[..received]);
        }
        Err(e) => {
            println!("recv function failed: {e:?}");
            std::process::exit(1)
        }
    }
    
    // Send chunks
    let start = Instant::now();
    for chunk in file_content.chunks(1024) {
        let frame = FrameHeader {
            id: FrameId::ChunkFrame,
            size: chunk.len() as u16,
            crc: crc.checksum(chunk)
        };
        
        println!("{:?}", frame);

        let mut frame = bincode::serialize(&frame).unwrap();
        frame.append(&mut chunk.to_vec());

        socket.send(&frame).expect("Failed to send header");

        match socket.recv(&mut rx_buf) {
            Ok(received) => {
                println!("Received {received} bytes {:?}", &rx_buf[..received]);
            }
            Err(e) => {
                println!("recv function failed: {e:?}");
                std::process::exit(1)
            }
        }
    }
    let duration = start.elapsed();
    let rate = file_content.len() as f32 / duration.as_secs_f32();

    println!("Time elapsed: {:?}, rate: {:?} B/s", duration, rate);
}
