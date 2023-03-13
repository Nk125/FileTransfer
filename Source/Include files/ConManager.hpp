#pragma once
#ifdef WINDOWS_SYSTEM
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// The most s####y cast seen in all over the world
// Source: https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
// So kids, that's how to do a mega unsafe cast only to complain with a super old socket convention
#define SOPT_CAST char*
#define SO_REUSEPORT 0
#define IOCN ioctlsocket
#define WR_FUNC send
#define RD_FUNC recv
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOPT_CAST int*
#define SOCKET int
#define INVALID_SOCKET -1
#define SD_BOTH SHUT_RDWR
#define IOCN ioctl
#define WR_FUNC write
#define RD_FUNC read
#endif
#include <chrono>
#include <ErrorMF.hpp>
#include <FileMF.hpp>
#include <vector>

class ConnectionMF {
public:
	typedef std::vector<char> Arr;

private:
	SOCKET socketId = INVALID_SOCKET, clientId = INVALID_SOCKET;
	size_t packetSize = 512;
	bool reListen = false;

	void saveMem(Arr&);
	bool blockwt(SOCKET, const Arr&);
	bool blockrd(SOCKET, Arr&);
	bool wt(SOCKET, const Arr&, size_t);
	int rd(SOCKET, Arr&, size_t);
	void serverHandle(SOCKET, const FileOperator::Directory&);
	void clientHandle(SOCKET, std::filesystem::path);

public:
	bool showProgress = false, showSpeed = false;

	ConnectionMF();
	~ConnectionMF();

	void setupClient(const char*, const char*, std::filesystem::path);
	void setupServer(unsigned short, const FileOperator::Directory&, bool, bool);
};
