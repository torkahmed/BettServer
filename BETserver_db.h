/* Filename: BETserver_db.h
 * Author: Ahmed Tourk
 * Date: 05.09.2018
 * Description:
 */

/*
 * INCLUDES
 */
#include "BETstd.h"

/*
 * TYPEDEFS
 */
typedef enum
{
    DB_FULL = 0,
    DB_OK = 1,
    DB_DUPLICATE =2
}tenuDBErrorCode;

/*
 * APIs
 */
PUBLIC tenuDBErrorCode DB_AddClientID(uint16_t clientID);
PUBLIC void DB_ClearIDList(void);
