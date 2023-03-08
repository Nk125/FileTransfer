cmake_minimum_required (VERSION 3.8)

cmake_policy (SET CMP0076 NEW)

project ("MyMFFileTransfer")

if (WIN32)
	add_compile_definitions (WINDOWS_SYSTEM)
else()
	add_compile_definitions (LINUX_SYSTEM)
endif()

add_executable (MyMFFileTransfer "this mf moves all the btchs.cpp")

target_sources (MyMFFileTransfer PUBLIC "Source files/mf dealing w cons.cpp" "Source files/mf dealing w files.cpp")

target_include_directories (MyMFFileTransfer PUBLIC "Include files")

set_property(TARGET MyMFFileTransfer PROPERTY CXX_STANDARD 17)