

#include <sys/socket.h>

#include "BETserver.h"


#define INVALID_SOCKET (-1)


PUBLIC int32_t SW_CreateServer(int32_t serverPort, int32_t maxPendingConnections);