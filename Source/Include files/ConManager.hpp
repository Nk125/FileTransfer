#pragma once
#ifdef WINDOWS_SYSTEM
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define SOCKET int
#define INVALID_SOCKET 0
#include <sys/socket.h>
#include <netdb.h>
#endif
#include <ErrorMF.hpp>
#include <vector>

class ConnectionMF {
private:
	SOCKET socketId = INVALID_SOCKET;

public:
	typedef std::vector<char> Arr;

	ConnectionMF();
	~ConnectionMF();

	bool open(const char*);
	bool listen(unsigned short);

};