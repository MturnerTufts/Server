#pragma once

typedef struct _SOCKET_INFORMATION
{
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	SOCKET Connected_Socket;
	OVERLAPPED Overlapp;
	DWORD BytesSEND;
	DWORD BytesRECV;
	bool Read;
	std::string Name;

	DWORD num;

}Socket_Information, * LPSOCKET_INFORMATION;

typedef struct _Socket_Setup
{
	SOCKET Listen_Socket;
	SOCKET Accept_Socket;
	FD_SET ReadSet;
	FD_SET WriteSet;
	DWORD Total_Sockets;
	LPSOCKET_INFORMATION SocketArray[10];
	DWORD Total_fd;
}Setup_Info, * LPSocket_Setup;