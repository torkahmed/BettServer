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
PUBLIC tenuDBErrorCode DB_AppendClientID(uint16_t clientID);
PUBLIC bool DB_AddBettingNumber(uint16_t clientID, uint32_t bettingNumber);
PUBLIC uint32_t DB_SelectWinningNumber(void);
PUBLIC void DB_ClearIDList(void);
