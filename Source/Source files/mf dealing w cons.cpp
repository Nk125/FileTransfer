#include <ConManager.hpp>

ConnectionMF::ConnectionMF() {
#ifdef WINDOWS_SYSTEM
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		Except("Couldn't start winsock", true);
	}
#endif

	struct sockaddr_in remote;

	socketId = socket(AF_INET, SOCK_STREAM, 0);

	memset(&remote, 0x00, sizeof(remote));
}

ConnectionMF::~ConnectionMF() {
#ifdef WINDOWS_SYSTEM
	closesocket(socketId);
#else
	close(socketId);
#endif
}