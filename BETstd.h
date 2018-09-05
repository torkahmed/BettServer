/* Filename: BETstd.h
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description: BET Component Standard Definitions
 */

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

#define BETSERVER_NUM_CLIENTS   (10U)
#define BETSERVER_NUM_MIN       (0xE0FFFF00)
#define BETSERVER_NUM_MAX       (0xE0FFFFAA)


/*
 * APIs
 */
