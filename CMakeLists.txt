cmake_minimum_required(VERSION 3.14)
project(Axon VERSION 0.1.0 LANGUAGES CXX)

add_library(axon INTERFACE)
target_compile_features(axon INTERFACE cxx_std_20)

target_include_directories(axon INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:inlcude>)

add_library(axon::axon ALIAS axon)

add_definitions(-DAXON_THROW_ON_LOCK_FAIL)