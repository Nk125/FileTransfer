#include <ConManager.hpp>

ConnectionMF::ConnectionMF() {
#ifdef WINDOWS_SYSTEM
	// WSAStartup can be called many times as needed
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) Except("Couldn't start winsock", true);
#endif

	if ((socketId = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) Except("Couldn't open socket", true);
}

ConnectionMF::~ConnectionMF() {
	std::cout << "Closing base socket and cleaning up\n";

#ifdef WINDOWS_SYSTEM
	closesocket(socketId);

	// In microsoft documentation pages says that, for each WSAStartup called a WSACleanup should be called too
	// So i'm including this in the destructor
	WSACleanup();
#else
	close(socketId);
#endif
}

void ConnectionMF::saveMem(Arr& a) {
	size_t end = std::find(a.begin(), a.end(), 0x00) - a.begin();
	a.resize(end);
	a.shrink_to_fit();
}

bool ConnectionMF::blockwt(SOCKET remSock, const Arr& buf) {
	Arr confirm(1, 0x00);

	if (!rd(remSock, confirm, 1)) {
		Except("Network error while reading confirm packet", (!reListen && true));
		return false;
	}

	if (!wt(remSock, buf, buf.size())) {
		Except("Network send data buffer error", (!reListen && true));
		return false;
	}

	return true;
}

bool ConnectionMF::blockrd(SOCKET remSock, Arr& buf) {
	Arr confirm(1, 0x01);

	if (!wt(remSock, confirm, 1)) {
		Except("Network error while sending confirm packet", true);
		return false;
	}

	unsigned long eofFlag = -1;
	size_t bufSz = buf.size(), loop = 1;

	do {
		if (!rd(remSock, buf, bufSz)) {
			Except("Network read data buffer error", true);
			return false;
		}
		else {
			buf.resize(bufSz * ++loop);
			IOCN(remSock, FIONREAD, &eofFlag);
		}
	} while (eofFlag > 0);

	saveMem(buf);
	return true;
}

bool ConnectionMF::wt(SOCKET remSock, const Arr& buf, size_t prBufSz) {
	bool r = (WR_FUNC(remSock, &buf[0], static_cast<int>(prBufSz)
#if WINDOWS_SYSTEM
		, 0
#endif
	) > 0);

	/*std::cout << "PKG OUT (bufsz: " << prBufSz << "): ";
	std::copy(buf.begin(), buf.end(), std::ostream_iterator<char>(std::cout, ""));
	std::cout << "\n";*/

	return r;
}

bool ConnectionMF::rd(SOCKET remSock, Arr& buf, size_t prBufSz) {
	bool r = (RD_FUNC(remSock, &buf[0], static_cast<int>(prBufSz)
#if WINDOWS_SYSTEM
		, 0
#endif
	) > 0);

	/*std::cout << "PKG IN (bufsz: " << prBufSz << "): [";
	std::copy(buf.begin(), buf.end(), std::ostream_iterator<char>(std::cout, ""));
	std::cout << "]\n";*/

	return r;
}

void ConnectionMF::serverHandle(SOCKET cliSocket, const FileOperator::Directory& files) {
	Arr gBuf(packetSize, 0x00);
	size_t allfs = files.size(), pkgSz = packetSize - 2;;

	{
		std::string fs = std::to_string(allfs);
		gBuf.assign(fs.begin(), fs.end());
		if (!blockwt(cliSocket, gBuf)) {
			Except("Failed to send file count", (!reListen && true));
			return;
		}
	}

	for (auto& p : files) {
		//int tf = 0;

		std::string snd = p.first.string() + ":" + std::to_string(p.second);

		gBuf.assign(snd.begin(), snd.end());

		saveMem(gBuf);

		if (!blockwt(cliSocket, gBuf)) {
			Except("Failed to send file pair", (!reListen && true));
			return;
		}

		FileOperator::Portal fileRead = FileOperator::init(FileOperator::In, std::filesystem::absolute(p.first).string().c_str());

		for (size_t i = 0; i <= p.second; i += pkgSz) {
			FileOperator::readChunk(fileRead, gBuf, pkgSz, i);

			if (!blockwt(cliSocket, gBuf)) {
				Except("Failed to send file chunk", (!reListen && true));
				return;
			}
		}

		fileRead.close();

		//std::cout << "\rFiles sent: " << ++tf << " Left: " << allfs - tf << " " << (100 * tf / allfs) << "% completed";
	}

	std::cout << "Finished TCP Connection\n";

	if (shutdown(cliSocket, SD_BOTH) != 0) Except("Failed to shutdown socket");
}

void ConnectionMF::clientHandle(SOCKET srvSocket, std::filesystem::path wp) {
	Arr gBuf(packetSize, 0x00);
	FileOperator::Directory files;

	if (wp.string() == ".") wp = std::filesystem::current_path();

	std::cout << "Writing files to this path: " << wp << "\n";

	if (!blockrd(srvSocket, gBuf)) {
		Except("Failed to receive file count", true);
	}

	size_t fs = std::stoull(std::string(gBuf.begin(), gBuf.end())), pkgSz = packetSize - 2;

	for (size_t i = 0; i < fs; i++) {
		gBuf.assign(packetSize, 0x00);

		if (!blockrd(srvSocket, gBuf)) {
			Except("Failed to receive file pair", true);
		}

		decltype(gBuf)::iterator ind;

		if ((ind = std::find(gBuf.begin(), gBuf.end(), ':')) != gBuf.end()) {
			std::filesystem::path fname(std::string(gBuf.begin(), ind));

			std::cout << "Writing file: " << fname << "\n";

			FileOperator::Portal fileWrite = FileOperator::init(FileOperator::Out, (std::filesystem::relative(wp).string()).c_str());
			
			size_t filesz = std::stoull(std::string(ind + 1, gBuf.end())), recvd = 0;

			while (recvd < filesz) {
				gBuf.assign(pkgSz, 0x00);

				if (!blockrd(srvSocket, gBuf)) {
					Except("Failed to receive file chunk", true);
				}

				FileOperator::writeChunk(fileWrite, gBuf, gBuf.size());

				recvd += pkgSz;
				//std::cout << "\r" << fname.string() << " progress: " << recvd << "/" << filesz << " (" << (100 * recvd / filesz) << "%)";
			}

			fileWrite.close();
		}
		else Except("A file pair couldn't be found", false);
	}

	std::cout << "Finished TCP Connection\n";

	if (shutdown(srvSocket, SD_BOTH) != 0) Except("Failed to shutdown socket");
}

void ConnectionMF::setupClient(const char* host, const char* port, std::filesystem::path wp) {
	if (!std::filesystem::create_directories(wp) && !std::filesystem::is_directory(wp)) Except("Working path isn't a directory", true);

	struct addrinfo hints = { 0 }, * ptr = { 0 };

	memset(&hints, 0x00, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if ((getaddrinfo(host, port, &hints, &ptr)) != 0) Except("Client failed getaddrinfo", true);

	if (connect(socketId, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) < 0) Except("Client failed connect", true);

	clientHandle(socketId, wp);
}

void ConnectionMF::setupServer(unsigned short port, const FileOperator::Directory& files, bool showFiles, bool rl) {
	if (files.empty()) {
		Except("The directory or file seems to be empty, aborting", true);
	}
	else if (showFiles) {
		std::cout << "You're transmiting this file(s):\n";
		int i = 0;
		for (auto& p : files) {
			std::cout << "  " << ++i << ".- " << p.first << " Size: " << float(float(p.second) / float(1000)) << " KB\n";
		}
	}

	int opt = 1; // Set to true the options

	if (setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (SOPT_CAST) &opt, sizeof(opt))) {
		Except("Socket reuse option didn't set successfully, may expect a connection error ");
	}

	struct sockaddr_in srv = { 0 };

	memset(&srv, 0x00, sizeof(srv));

	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = INADDR_ANY; // Should work with localhost
	srv.sin_port = htons(port);

	if (bind(socketId, (struct sockaddr*) &srv, sizeof(srv)) < 0) Except("Server bind failed", true);

	if (listen(socketId, SOMAXCONN) < 0) Except("Server listen failed", true);

	int srvaddrlen = sizeof(srv);

	reListen = rl;

	do {
		std::cout << "Waiting connection\n";

		if ((clientId = accept(socketId, (struct sockaddr*)&srv, (socklen_t*)&srvaddrlen)) == INVALID_SOCKET) {
			Except("Server accept failed", true);
		}

		serverHandle(clientId, files);
	} while (reListen);
}
