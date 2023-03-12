#include <argh.h>
#include <ConManager.hpp>
#include <FileMF.hpp>
#ifdef WINDOWS_SYSTEM
#pragma comment(lib, "ws2_32")
#endif

int main(int argc, char* argv[]) {
	unsigned short port = 0;
	ConnectionMF myniggconn;
	argh::parser argmf(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

	if (argc <= 1 || argmf["--help"]) {
		std::cout << (R"""(
Usage:
	--help - This help

	For server mode:
		-l/--listen <port> - This activates the server mode and sets the port
		-tp/--transmit-path <path> - Transmits all the files in that folder or the file specified in the path
		-ex/--exclude <regex> - Excludes all the files/directories with the regex specified
		-in/--include <regex> - Includes only the files/directories if regex match
		-fsmn/--filesize-min <number> - The minimum size that a file should have to be included
		-fsmx/--filesize-max <number> - The maximum size allowed for a file to be included
		-sf/--show-files - Show the files you're sharing
		-rl/--re-listen - After dispatching one client relistens to send files to another one

	For client mode:
		-h/--host <addr> - This activates the client mode and sets the host to connect
		-rp/--remote-port - Sets the remote port to connect
		-wp/--write-path - Sets the path to write files, by default is the working directory
)""");
		return 0;
	}

	if ((argmf({ "-l", "--listen" }) >> port)) {
		std::string tp, exstr, instr;
		bool sf = false, rl = false;
		size_t min, max;
		
		if ((tp = argmf({ "-tp", "--transmit-path" }).str()).empty()) {
			Except("No transmit path set", true);
		}
		argmf({ "-ex", "--exclude" }, "") >> exstr;
		argmf({ "-in", "--include" }, ".*") >> instr;
		argmf({ "-fsmn", "--filesize-min" }, 0) >> min;
		argmf({ "-fsmx", "--filesize-max" }, -1) >> max;
		sf = argmf[{ "-sf", "--show-files" }];
		rl = argmf[{ "-rl", "--re-listen" }];

		std::regex ex, in;

		try {
			ex = std::regex(exstr),
			in = std::regex(instr);
		}
		catch (...) {
			Except("Bad regex passed", true);
		}

		std::cout << "Setup server started\n";

		myniggconn.setupServer(port, FileOperator::makeFileList(tp, ex, in, min, max), sf, rl);
	}
	else {
		std::string host, wp;

		if (!(argmf({ "-h", "--host" }) >> host)) Except("You need to provide a host", true);
		if (!(argmf({ "-rp", "--remote-port" }) >> port)) Except("Remote port isn't set", true);
		argmf({ "-wp", "--write-path" }, ".") >> wp;

		std::cout << "Setup client started\n";

		myniggconn.setupClient(host.c_str(), std::to_string(port).c_str(), wp);
	}
}
