/* Filename: BETstd.h
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description: BET Component Standard Definitions
 */
#ifndef BETSTD_H
#define BETSTD_H
/*
 * INCLUDES
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * MACROS
 */
#define LOCAL static
#define PUBLIC
#define UNUSED(x) (void)(x)

#define BETSERVER_PORT          (2222)

#define BETSERVER_NUM_CLIENTS   (64500U)
#define BETSERVER_NUM_MIN       (htonl(0xE0FFFF00))
#define BETSERVER_NUM_MAX       (htonl(0xE0FFFFAA))


#define PROTOCOL_VERSION          (0x1)
/* MESSAGE TYPES */
#define BETSERVER_OPEN            (0x1)
#define BETSERVER_ACCEPT          (0x2)
#define BETSERVER_BET             (0x3)
#define BETSERVER_RESULT          (0x4)


/*
 * MESSAGE TYPEDEFs
 */

typedef struct sBetServerMessageHeader
{
    uint8_t u8Version;
    uint8_t u8Length;
    uint8_t u8Type;
    uint16_t u16ClientID;
}mBetServerMessageHeader;

typedef struct sBetServerMessageAccept
{
    uint32_t u32BetLowerBounds;
    uint32_t u32BetUpperBounds;
}mBetServerMessageAccept;

typedef struct sBetServerMessageBet
{
    uint32_t u32BettingNumber;
}mBetServerMessageBet;

typedef struct sBetServerMessageResult
{
    uint8_t u8Status;
    uint32_t u32WinningNumber;
}mBetServerMessageResult;


#endif /* BETSTD_H */
