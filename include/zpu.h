#ifndef ZPU_H
#define ZPU_H

#include "zpu_commands.h"

#define ZPU_ERR_OK            0x00
#define ZPU_ERR_PARSEERROR    0x01
#define ZPU_ERR_DEVFILEOPEN   0x02
#define ZPU_ERR_MMAP          0x03
#define ZPU_ERR_MEMCOPY       0x04
#define ZPU_ERR_COULDNOTSTOP  0x05
#define ZPU_ERR_COULDNOTSTART 0x06

int zpu_load_from_file(char* filename);
int zpu_stop();
int zpu_start();

#endif
