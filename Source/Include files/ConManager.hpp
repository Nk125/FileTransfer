#pragma once
#ifdef WINDOWS_SYSTEM
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define SOCKET int
#define INVALID_SOCKET -1
#include <sys/socket.h>
#include <netdb.h>
#endif
#include <ErrorMF.hpp>
#include <vector>

class ConnectionMF {
private:
	SOCKET socketId = INVALID_SOCKET, clientId = INVALID_SOCKET;

	void serverHandle(SOCKET);

public:
	typedef std::vector<char> Arr;

	ConnectionMF();
	~ConnectionMF();

	void setupClient(const char*, unsigned short);
	void setupServer(unsigned short);
};
