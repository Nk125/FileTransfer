add_requires("argh 1.3.2")

target("FileTransfer")
	set_kind("binary")
	set_languages("c++17")
	add_files("Source/Source files/*.cpp", "Source/this mf moves all the btchs.cpp")
	add_includedirs("Source/Include files")
	add_packages("argh")

	if is_plat("windows") then
		add_defines("WINDOWS_SYSTEM")
	else
		add_defines("LINUX_SYSTEM")
	end