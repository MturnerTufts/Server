#include <iostream>
#include "Server_Setup.h"



int main()
{


	LPSocket_Setup S_Info= (LPSocket_Setup)new int(sizeof(LPSocket_Setup));
	S_Info->Total_Sockets = 0;
	//*S_Info->SocketArray = (LPSOCKET_INFORMATION) new int(sizeof(LPSOCKET_INFORMATION)*80);
	SOCKADDR_IN InternetAddr = { 0 };
	//so that reads first
	
	NetWorkStart(S_Info,InternetAddr);
	

}