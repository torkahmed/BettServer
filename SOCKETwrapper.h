/* Filename: SOCKETwrapper.h
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description:
 */

/*
 * INCLUDES
 */
#include <sys/socket.h>

#include "BETstd.h"


/*
 * MACROS
 */
#define INVALID_SOCKET (-1)


/*
 * APIs
 */

PUBLIC int32_t SW_CreateServer(int32_t serverPort, int32_t maxPendingConnections);
PUBLIC void SW_TearConnection(int32_t socketDescriptor);
PUBLIC bool SW_ConnectToServer(int32_t *socketDescriptor, uint32_t ip, uint16_t port);
PUBLIC bool SW_RecvMessage(int32_t peerSocket, char *recvBuffer, size_t numBytesToRecv);
