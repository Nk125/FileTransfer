cmake_minimum_required (VERSION 3.8)

project ("FileTransfer")

if (WIN32)
	add_compile_definitions (WINDOWS_SYSTEM)
else()
	add_compile_definitions (LINUX_SYSTEM)
endif()

add_executable (FileTransfer "FileTransfer.cpp")

target_sources (FileTransfer PUBLIC "${FileTransfer_SOURCE_DIR}/Source/TCPcon.cpp")

target_include_directories (FileTransfer PUBLIC "Include")

set_property(TARGET FileTransfer PROPERTY CXX_STANDARD 17)