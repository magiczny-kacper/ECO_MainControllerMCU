# Main Controller MCU
Energy Consumption Optimizer main MCU. It is suposed to manage all communication with devices, an decide how much power should be given to each heater. 
# ECO - Energy Consumption Optimiser
Energy Consumptio Optimiser, ECO, is small system taht the mian point of work is to optimise energy consumption in households with solar power plants and electric or mixed water heating.
The system contains of:
- **Main Controller** (which software is in this repository)
- **Digital I/O module** (hardware only), as expansion module for Main Controller
- **Isolation Module** (hardware only) and **Triac Control Module** (hardware and software), as an expansions modules
- **Energy Meter Reader** module as remote module, mounted on energy meter, for reading it via optical port and IEC1107 protocol. Wire or wireless communication with mian module
- **Wireless I/O Module** as multipurpose I/O expansion
# Software description
### RTOS Tasks
This software operates under FreeRTOS, to easyli manage tasks of which there are several. The tasks are:
- **DataLogTask** - it handles events saving from queue to which other tasks are logging events during runtime.
- **DisplayTask** - task handling two nextion HMIs data exchange.
- **EthernetTask** - task which handles ethernet communication.
- **RadioTask** - Wireless communication task.
- **RegulationTask** - the most importatnt of the tasks, it handles energy consumption otimisation.
### Features
This device have few features implemented, or to be implemented.
- **Modbus RTU master** - it is able to communicate with devices supporting Modbus RTU protocol, via RS485 port, which enables reading temperatures, and grid parameters from external devices. This data is then used for regulation task.
- **Modbus TCP server** - the presence of ethernet port gives very wide possibilities of communication with the world. One of implemented is Modbus TCP server. There are yet no registers defined to read from main controller (to be defined in the future). Moreover, this device operates as Modbus TCP server <-> Modbus RTU master bridge. Requests send over TCP with slave ID other than 0 and 255 are forwarded to RS485 port (in proper for RTU format), and received response is then forwarded back from RS485 port to ethernet port.
- **Telnet CLI** - simple telnet CLI was implemented, which allows to i.e. read events from flash memory, get device state or set date and time.
- **Data logging** - device is equipped with external flash memory, which is used to store events, that tasks are generating in runtime. Example events logged: RTC update, Telenet connection, Telnet disconnection.
