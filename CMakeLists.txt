cmake_minimum_required(VERSION 3.28)
project(PartyPeople)

set(CMAKE_CXX_STANDARD 26)

find_library(USOCKETS_LIBRARY uSockets.a PATHS /usr/local/lib REQUIRED)
find_package(ZLIB REQUIRED)

set(source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB source_files
        ${source_dir}/*.cpp
)

add_executable(PartyPeople ${source_files})

target_include_directories(${PROJECT_NAME} PUBLIC ${source_dir}/include)
target_link_libraries(${PROJECT_NAME} ${USOCKETS_LIBRARY})
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)

