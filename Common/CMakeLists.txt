if(POLICY CMP0079 )
  cmake_policy(SET CMP0079 NEW)
endif()

set(common_sources
    ${CMAKE_SOURCE_DIR}/Common/external/printf/printf.c
    ${CMAKE_SOURCE_DIR}/Common/external/RingBuffer/Src/ring.c
    ${CMAKE_SOURCE_DIR}/Common/drivers/W25Q64/W25Q64_Flash.c
    ${CMAKE_SOURCE_DIR}/Common/flash/flash_map_backend/flash_map_backend.c
)

set(common_includes
    ${CMAKE_SOURCE_DIR}/Common/external/printf
    ${CMAKE_SOURCE_DIR}/Common/external/RingBuffer/Src
    ${CMAKE_SOURCE_DIR}/Common/flash
    ${CMAKE_SOURCE_DIR}/Common/drivers/W25Q64
)

# add_library(W25Q64 STATIC drivers/W25Q64/W25Q64_Flash.c)
# target_include_directories(W25Q64 PUBLIC "${CMAKE_CURRENT_LIST_DIR}/drivers/W25Q64")

add_subdirectory(external/mcuboot/boot/bootutil)
add_subdirectory(external/mcuboot/ext/mbedtls)
target_include_directories(bootutil PUBLIC ${common_includes})
# target_link_libraries(bootutil mbedtls)