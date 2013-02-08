#define ZPU_C
#include "config.h"
#include "zpu.h"
#include "cintelhex.h"

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEBUG

#ifdef DEBUG
# define OUT_DBG(msg, ...) printf(msg, ##__VA_ARGS__)
#else
# define OUT_DBG(msg, ...)
#endif

#ifndef HAVE_SNPRINTF
# define snprintf(s,l,f,...) sprintf(s,f,##__VA_ARGS__)
#endif

#define ZPU_SET_ERROR(errno, error, ...) \
	zpu_last_errno = errno; \
	zpu_last_error = (char*) malloc(128); \
	snprintf(zpu_last_error, 128, error, ##__VA_ARGS__);
#define ZPU_SET_ERROR_RETURN(errno, error, ...) \
	ZPU_SET_ERROR(errno, error, ##__VA_ARGS__); \
	return errno;

#define ZPU_OPEN_RETURN(fd) if (ZPU_OPEN_RW(fd) <= 0) { \
	ZPU_SET_ERROR_RETURN(ZPU_ERR_DEVFILEOPEN, \
		"Could not open file %s.", ZPU_DEV_FILENAME); }

zpu_error_t zpu_load_from_file(char* filename)
{
	ihex_recordset_t *rs;
	char             *addr;
	
	#ifndef ZPU_MOCK
	int               fd;
	#endif
	
	if (zpu_stop() != ZPU_ERR_OK)
	{
		return zpu_last_errno;
	}
	
	if ((rs = ihex_rs_from_file(filename)) == NULL)
	{
		ZPU_SET_ERROR_RETURN((ZPU_ERR_PARSEERROR << 16) + ihex_errno(),
			"Could not parse HEX file %s: %s", filename, ihex_error());
	}
	
	#ifdef ZPU_MOCK
	addr = (char*) malloc(ZPU_RAM_SIZE);
	#else
	
	ZPU_OPEN_RETURN(fd);
	
	if ((addr = mmap(NULL, ZPU_RAM_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0)) == NULL)
	{
		ZPU_SET_ERROR(ZPU_ERR_MMAP, "Could not map ZPU memory.");
		goto mmap_failed;
	}
	#endif
	
	if (ihex_mem_copy(rs, addr, ZPU_RAM_SIZE) != 0)
	{
		ZPU_SET_ERROR((ZPU_ERR_MEMCOPY << 16) + ihex_errno(),
			"Could not copy program into RAM: %s", ihex_error());
		goto memcopy_failed;
	}
	
	printf("0x0400: 0x%02x\n", addr[0x400]);
	
	#ifndef ZPU_MOCK
	munmap(addr, ZPU_RAM_SIZE);
	close(fd);
	#endif
	
	if (zpu_start() != ZPU_ERR_OK)
	{
		return zpu_last_errno;
	}
	
	return ZPU_ERR_OK;
	
	memcopy_failed:
	#ifndef ZPU_MOCK
		munmap(addr, ZPU_RAM_SIZE);
	mmap_failed:
		close(fd);
	#endif
	return zpu_last_errno;
}

int zpu_stop()
{
	#ifndef ZPU_MOCK
	int fd;
	
	ZPU_OPEN_RETURN(fd);
	
	if (ioctl(fd, RAGGED_ZPU_STOP) < 0)
	{
		ZPU_SET_ERROR(ZPU_ERR_COULDNOTSTOP, "Could not stop ZPU.");
		goto ioctl_failed;
	}
	
	close(fd);
	
	return ZPU_ERR_OK;
	
	ioctl_failed:
		close(fd);
	
	return zpu_last_errno;
	#else
	return ZPU_ERR_OK;
	#endif
}

int zpu_start()
{
	#ifndef ZPU_MOCK
	int fd;
	
	ZPU_OPEN_RETURN(fd);
	
	if (ioctl(fd, RAGGED_ZPU_START) < 0)
	{
		ZPU_SET_ERROR(ZPU_ERR_COULDNOTSTART, "Could not start ZPU.");
		goto ioctl_failed;
	}
	
	close(fd);
	
	return ZPU_ERR_OK;
	
	ioctl_failed:
		close(fd);
	
	return zpu_last_errno;
	#else
	return ZPU_ERR_OK;
	#endif
}

zpu_error_t zpu_errno()
{
	return zpu_last_errno;
}

char* zpu_error()
{
	return zpu_last_error;
}
