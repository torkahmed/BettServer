
/*
 * INCLUDES
 */
#include <sys/socket.h>
#include <unistd.h>

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
