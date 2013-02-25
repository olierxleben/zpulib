#ifndef ZPU_H
#define ZPU_H

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "zpu_commands.h"

// CONSTANT DEFINITIONS

#define ZPU_ERR_OK            0x00
#define ZPU_ERR_PARSEERROR    0x01
#define ZPU_ERR_DEVFILEOPEN   0x02
#define ZPU_ERR_MMAP          0x03
#define ZPU_ERR_MEMCOPY       0x04
#define ZPU_ERR_COULDNOTSTOP  0x05
#define ZPU_ERR_COULDNOTSTART 0x06

#define ZPU_DEV_FILENAME "/dev/zpu"
#define ZPU_RAM_SIZE     8192 // bytes
	
#define ZPU_OPEN_RW(fd) (fd = open(ZPU_DEV_FILENAME, O_RDWR,   S_IWUSR|S_IRUSR))
#define ZPU_OPEN_R(fd)  (fd = open(ZPU_DEV_FILENAME, O_RDONLY, S_IRUSR))
#define ZPU_OPEN_W(fd)  (fd = open(ZPU_DEV_FILENAME, O_WRONLY, S_IWUSR))

// TYPE DEFINITIONS

typedef unsigned int uint_t;
typedef uint_t       zpu_error_t;

// GLOBAL VARIABLES

#ifdef ZPU_C
static zpu_error_t zpu_last_errno = 0;
static char*       zpu_last_error = NULL;
#else
extern zpu_error_t zpu_last_errno; //!< Error code of last error.
extern char*       zpu_last_error; //!< Description of last error.
#endif

// METHOD DECLARATIONS

/// Load an Intel HEX formatted program into the ZPU
/** This method loads a new program into the ZPU. The input file needs
 *  to be an Intel HEX formatted binary.
 *  Before loading the program, the ZPU is set into "reset" mode. After
 *  that, the new program is parsed and copied into the ZPU's ROM and
 *  the ZPU is set back into regular mode.
 *  @param filename The filename of the input Intel HEX file.
 *  @return         ZPU_ERR_OK on success, otherwise an error code. */
zpu_error_t zpu_load_from_file(char* filename);

/// Halt the ZPU.
/** This method sets the ZPU into "reset" mode.
 *  @return ZPU_ERR_OK on success, otherwise an error code. */
int zpu_stop();

/// Start the ZPU, if halted.
/** This method sets the ZPU back into regular mode, if it was stopped
 *  earlier. 
 *  @return ZPU_ERR_OK on success, otherwise an error code. */
int zpu_start();

/// Returns the last error number.
zpu_error_t zpu_errno();

/// Returns the last error.
char* zpu_error();

#endif
