#include <ConManager.hpp>

ConnectionMF::ConnectionMF() {
#ifdef WINDOWS_SYSTEM
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		Except("Couldn't start winsock", true);
	}
#endif

	if ((socketId = socket(AF_INET, SOCK_STREAM, 0) == INVALID_SOCKET) {
		Except("Couldn't open socket", true);
	}
}

ConnectionMF::~ConnectionMF() {
#ifdef WINDOWS_SYSTEM
	closesocket(socketId);
#else
	close(socketId);
#endif
}

void ConnectionMF::setupClient(const char* host, unsigned short port) {
	
}

void ConnectionMF::setupServer(unsigned short port) {
	int opt = 1; // Set to true the options

	if (setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		Except("Socket reuse option didn't set successfully, may expect a connection error ");
	}

	struct socketaddr_in srv;

	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = INADDR_ANY; // Should work with localhost
	srv.sin_port = htons(port);

	if (bind(socketId, (struct sockaddr*) srv, sizeof(srv)) < 0) {
		Except("Server bind failed", true);
	}

	if (listen(socketId, 5) < 0) {
		Except("Server listen failed", true);
	}

	int srvaddrlen = sizeof(srv);

	if ((clientId = accept(socketId, (struct sockaddr*) &srv, (socklen_t*) &srvaddrlen))) {
		Except("Server accept failed");
		return;
	}

        serverHandle(clientId);
}
