cmake_minimum_required(VERSION 3.10)

link_directories(${common_includes})

project(ECO_MainMCU C ASM)

add_subdirectory(Common)
add_subdirectory(ECO_MainMCU_App)