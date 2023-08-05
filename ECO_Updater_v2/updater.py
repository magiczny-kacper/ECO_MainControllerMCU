import socket
import sys
import logging
from enum import Enum
from protobuf import swdl_pb2
import crc

TX_PORT = 3400
RX_PORT = 4300

SWDLBlock = swdl_pb2.InfoFrame.BlockId
FrameType = swdl_pb2.FrameHeader.FrameId
ResponseFrame = swdl_pb2.ResponseFrame

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)

formatter = logging.Formatter('%(levelname)s:%(name)s:%(message)s')

ch.setFormatter(formatter)
logger.addHandler(ch)

class Updater:
    
    def __init__(self):
        self.__tx_data = None
        self.__rx_data = None
        self.__target_ip = None
        self.__file = None
        self.__socket = None
        
        logger.debug("Initializing socket")
        self.__socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.__socket.bind(("0.0.0.0", 3400))
        self.__socket.settimeout(5.0)
        return

    def __send_packet(self):
        if self.__target_ip == None:
            raise Exception("Target IP is not set")
        else:
            logger.debug("Sending {len} bytes to {addr}".
                format(len = len(self.__tx_data), addr = self.__target_ip))
            self.__socket.sendto(self.__tx_data, (self.__target_ip, TX_PORT))
        return

    def __receive_packet(self) -> bool:
        packet = self.__socket.recvfrom(1024)
        if not packet:
            logger.error("No packet received")
            return False
        else:
            self.__rx_data = packet[0]
            logger.debug("Received {len} bytes from {addr}".
                    format(len = len(packet[0]), addr = packet[1]))
        return True
    
    def __do_transaction(self) -> bool:
        status = False
        retry_count = 0
        
        while (not status) and (retry_count < 5):
            status = self.__send_packet()
            if status:
                self.__receive_packet()
            
                response = ResponseFrame()
                response.ParseFromString(self.__rx_data)
                status =  response.status == ResponseFrame.Status.OK
            
            retry_count += 1
            
        if not status:
            logger.warning("Transaction failed")
        return status
    
    def __read_file(self, file: str) -> None:
        with open(file, 'rb') as f:
            self.__file = bytearray(f.read())
        return
    
    def divide_chunks(data, size):     
        # looping till length l
        for i in range(0, len(data), size): 
            yield data[i:i + size]
        
    def init_update(self, target_addr: str, updated_block: SWDLBlock, file: str):
        self.__target_ip = target_addr
        self.__read_file(file)
        return
    
    def update(self) -> None:
        # Prepare initial frame
        frame = swdl_pb2.SWDLFrame()
        frame.header.id = FrameType.INFO_FRAME
        frame.info.block_id = updated_block
        frame.info.file_size = len(self.__file)
        
        self.__tx_data = frame.SerializeToString()
        self.__do_transaction()
        print(self.divide_chunks(self.__file, 4096))
        

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    upd = Updater()
    upd.init_update("127.0.0.1", SWDLBlock.MCU_APPLICATION, "ECO_MainMCU_App_signed.bin")
    upd.update()