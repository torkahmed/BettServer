/* Filename: SOCKETwrapper.h
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description:
 */

/*
 * INCLUDES
 */
#include <sys/socket.h>

#include "BETserver.h"


/*
 * MACROS
 */
#define INVALID_SOCKET (-1)


/*
 * APIs
 */

PUBLIC int32_t SW_CreateServer(int32_t serverPort, int32_t maxPendingConnections);
PUBLIC void SW_ShutdownServer(int32_t socketDescriptor);
