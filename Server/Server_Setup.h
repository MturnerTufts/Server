#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include<stdio.h>
#include<iostream>
#include<string>

#pragma comment(lib,"ws2_32.lib")
#define PORT 8080
#define DATA_BUFSIZE 8888
#define FD_SETSIZE 10


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
bool Start_Winsock();
bool Server_Socket_setup(LPSocket_Setup Setup_Information, SOCKADDR_IN& InternetAddr);
bool Set_Read_Write(LPSocket_Setup S_Info);
bool Arriving_Connections(LPSocket_Setup S_Info);
void Send_Recv(LPSocket_Setup S_Info);
void Send_Mes(LPSOCKET_INFORMATION SocketInfo, LPSocket_Setup S_Info, DWORD Index);
bool CreateSocketInformation(LPSocket_Setup S_Info);
void FreeSocketInformation(LPSocket_Setup S_Info, DWORD Index);



bool Start_Winsock()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2,2),&wsadata)!=0)
	{
		printf("failed to start winsock");
		return false;
	}
	return true;
}

bool Server_Socket_setup(LPSocket_Setup Setup_Information, SOCKADDR_IN& InternetAddr)
{
	SOCKET *ListenSocket=&Setup_Information->Listen_Socket;
	
	u_long Non_Block = 1;
	if ((*ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("Can't connect listen socket");
		return false;
	}
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(PORT);
	if (bind(*ListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		printf("bind() failed");
		return false;
	}
	if (listen(*ListenSocket, 5))
	{
		printf("listen has failed");
		return false;
	}
	
	if (ioctlsocket(*ListenSocket, FIONBIO, &Non_Block) == SOCKET_ERROR)
	{
		printf("failed ioctsocket");
		return false;
	}

	printf("Server_Setup completed");
	return true;
}

bool Set_Read_Write(LPSocket_Setup S_Info)
{
	FD_ZERO(&S_Info->ReadSet);
	FD_ZERO(&S_Info->WriteSet);
	FD_SET(S_Info->Listen_Socket, &S_Info->ReadSet);

	for (int i = 0; i <S_Info->Total_Sockets; i++)
	{

		if (S_Info->SocketArray[i]->Read == true)
			FD_SET(S_Info->SocketArray[i]->Socket, &S_Info->ReadSet);
		else
			FD_SET(S_Info->SocketArray[i]->Socket, &S_Info->WriteSet);
	}
	if ((S_Info->Total_fd = select(0, &S_Info->ReadSet, &S_Info->WriteSet, NULL, NULL)) == SOCKET_ERROR)
	{
		printf("Select retruned an error");
		return false;
	}
	printf("\nThe total_fd is %i \n", S_Info->Total_fd);


	return true;
}

bool Arriving_Connections(LPSocket_Setup S_Info)
{
	if (FD_ISSET(S_Info->Listen_Socket, &S_Info->ReadSet))
	{
		--S_Info->Total_fd;
		if ((S_Info->Accept_Socket = accept(S_Info->Listen_Socket, NULL, NULL)) != INVALID_SOCKET);
		{
			ULONG Non_Block = 1;
			if (ioctlsocket(S_Info->Accept_Socket, FIONBIO, &Non_Block) == SOCKET_ERROR)
			{
				printf("ioctsocket(FIONBIO) failed error ");
				return false;
			}
			if (CreateSocketInformation(S_Info) == false)
			{
				printf("CreateSocketInfomration failed");
				return false;
			}
			return true;
		}
		return false;
		
		
	}
}

void Send_Recv(LPSocket_Setup S_Info)
{
	DWORD Flags = 0;
	for (int i = 0; S_Info->Total_fd > 0 && i < S_Info->Total_Sockets;i++)
	{
		LPSOCKET_INFORMATION SocketInfo = S_Info->SocketArray[i];
		if (FD_ISSET(SocketInfo->Socket, &S_Info->ReadSet))
		{

			printf("made to recieve");
			printf("\n");
			printf("\n");
			--S_Info->Total_fd;
			SocketInfo->DataBuf.buf = SocketInfo->Buffer;
			SocketInfo->DataBuf.len = DATA_BUFSIZE;
			if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SocketInfo->BytesRECV, &Flags, NULL, NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					FreeSocketInformation(S_Info, i);
				}
			}
			else
			{
				//printf(SocketInfo->DataBuf.buf);
				if (SocketInfo->BytesRECV == 0)
				{

					FreeSocketInformation(S_Info,i);
				
				}

			}
			printf("Buffer:");
			printf(SocketInfo->DataBuf.buf);
			printf("\n");
			SocketInfo->BytesSEND = 0;
		}
		Send_Mes(SocketInfo, S_Info, i);
	}
}


void Send_Mes(LPSOCKET_INFORMATION SocketInfo, LPSocket_Setup S_Info, DWORD Index)
{
	DWORD Flags=0;
	if (FD_ISSET(SocketInfo->Socket, &S_Info->WriteSet))
	{
		S_Info->Total_fd--;
		std::cout << "Going throuhg writeset";
		printf("\n");
		SocketInfo->DataBuf.buf = SocketInfo->Buffer + SocketInfo->BytesSEND;
		SocketInfo->DataBuf.len = SocketInfo->BytesRECV - SocketInfo->BytesSEND;
		if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SocketInfo->BytesSEND, Flags, NULL, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				printf("WSASEND failed");
				FreeSocketInformation(S_Info,Index);
			}

		}
		else
		{
			printf("\n");
			printf("it sent");
			printf("\n");
			
		}
		SocketInfo->BytesRECV = 0;
		printf("bytes sent: ");
	//	std::cout << SocketInfo->BytesSEND;
		printf("\n");
		
	}
	//std::cout << SocketInfo->BytesRECV << "\n" << SocketInfo->BytesSEND << "\n";
	if (SocketInfo->BytesRECV >= SocketInfo->BytesSEND)
	{
		SocketInfo->Read = false;
		std::cout << "false";
	}
	else
	{
		std::cout << "true";
		SocketInfo->Read = true;
	}
	
	
	
	//std::cout << "\n" << SocketInfo->BytesRECV << SocketInfo->BytesSEND << "\n";
}

bool CreateSocketInformation(LPSocket_Setup S_Info)
{
	LPSOCKET_INFORMATION SI;
	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(Socket_Information))) == NULL)
	{
		printf("GlobalAlloc() failed with error \n");
		return false;
	}
	SI->Socket = S_Info->Accept_Socket;
	

	printf("BytesSend");
	printf("%s",SI->BytesSEND);
	printf("\n");
	SI->Read = true;
	SI->BytesRECV = 0;
	SI->BytesSEND = 0;
	S_Info->SocketArray[S_Info->Total_Sockets] = SI;
	++S_Info->Total_Sockets;
	printf("total sockets %i", S_Info->Total_Sockets);
	printf("\n");
	printf("a new socket has joined");
	printf("\n");


	printf("\n");
	return true;
	
}

void FreeSocketInformation(LPSocket_Setup S_Info, DWORD Index)
{
	LPSOCKET_INFORMATION Socket_Information = S_Info->SocketArray[Index];

	closesocket(Socket_Information->Socket);
	GlobalFree( Socket_Information);
	for (DWORD i = Index; i < S_Info->Total_Sockets; i++)
	{
		S_Info->SocketArray[i] = S_Info->SocketArray[i + 1];
	}
	S_Info->Total_Sockets--;
	printf("\n someone has left \n");
}

void NetWorkStart(LPSocket_Setup S_Info, SOCKADDR_IN InternetAddr)
{
	Start_Winsock();
	Server_Socket_setup(S_Info, InternetAddr);

	while (true) {
		printf("\n");
		printf("the cycle beginsa again");
		printf("\n");
		Set_Read_Write(S_Info);
		Arriving_Connections(S_Info);
		Send_Recv(S_Info);
	}
}

	
