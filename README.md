# Main Controller MCU
Energy Consumption Optimizer main MCU. It is suposed to manage all communication with devices, an decide how much power should be given to each heater. 
# ECO - Energy Consumption Optimiser
Energy Consumptio Optimiser, ECO, is small system taht the mian point of work is to optimise energy consumption in households with solar power plants and electric or mixed water heating.
The system contains of:
- Main Controller (which software is in this repository)
- Digital I/O module (hardware only), as expansion module for Main Controller
- Isolation Module (hardware only) and Triac Control Module (hardware and software), as an expansions modules
- Energy Meter Reader module as remote module, mounted on energy meter, for reading it via optical port and IEC1107 protocol. Wire or wireless communication with mian module
- Wireless I/O Module as multipurpose I/O expansion
# Software description
This software operates under FreeRTOS, to easyli manage tasks of which there are several. The tasks are:
- DataLogTask - it handles events saving from queue to which other tasks are logging events during runtime.
- DisplayTask - task handling two nextion HMIs data exchange.
- EthernetTask - task which handles ethernet communication.
- RadioTask - Wireless communication task.
- RegulationTask - the most importatnt of the tasks, it handles energy consumption otimisation.