#include "SOCKETwrapper.h"


PUBLIC int32_t SW_CreateServer(int32_t serverPort, int32_t maxPendingConnections)
{
	int32_t serverSocket = INVALID_SOCKET;
	struct sockaddr_in serverSockAddr;

  /* Step 1: Create a TCP Socket */
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(serverSocket < 0)
	{
		fprintf(stderr, "[E] Could Not Create Server Socket\n");
		return INVALID_SOCKET;
	}

  /* Step 2: Bind */
  serverSockAddr.sin_family = AF_INET;
  serverSockAddr.sin_port = htons(serverPort);
  serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if ( bind(serverSocket, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)) < 0 )
  {
		fprintf(stderr, "[E] Could Not Bind Server Socket\n");
		return INVALID_SOCKET;
  }

  /* Step 3: Listen */
  if ( listen(serverSocket, maxPendingConnections) < 0)
  {
  	fprintf(stderr, "[E] Could Not Listen on Server Socket\n");
    return INVALID_SOCKET;
  }

  fprintf(stdout, "[I] Server Socket Created (%d) on Port (%d)\n", serverSocket, serverPort);
	return serverSocket;
}