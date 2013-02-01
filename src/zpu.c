#include "zpu.h"
#include "cintelhex.h"

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

int zpu_load_from_file(char* filename)
{
	int               e = 0;
	ihex_recordset_t *rs;
	int               fd;
	void             *addr;
	
	if ((rs = ihex_rs_from_file(filename)) == NULL)
	{
		printf("Konnte HEX-Datei nicht parsen.\n");
		printf(ihex_error());
		return ZPU_ERR_PARSEERROR;
	}
	
	if ((fd = open("/dev/zpu", O_RDWR, S_IWUSR)) < 0)
	{
		e = ZPU_ERR_DEVFILEOPEN;
		goto open_failed;
	}
	
	if ((addr = mmap(NULL, 8192, PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
	{
		printf("Konnte Speicher nicht mappen.\n");
		e = ZPU_ERR_MMAP;
		goto mmap_failed;
	}
	
	if (ihex_mem_copy(rs, addr, 8192) != 0)
	{
		printf("Konnte Speicher nicht befüllen.\n");
		printf(ihex_error());
		e = ZPU_ERR_MEMCOPY;
		goto memcopy_failed;
	}
	
	munmap(addr, 8192);
	close(fd);
	
	return ZPU_ERR_OK;
	
	memcopy_failed:
		munmap(addr, 8192);
	mmap_failed:
		close(fd);
	open_failed:
		// ihex_rs_free(rs);
	return e;
}

int zpu_stop()
{
	int e;
	int fd;
	
	printf("Halte ZPU an.\n");
	
	if ((fd = open("/dev/zpu", O_RDWR, S_IWUSR)) < 0)
	{
		e = ZPU_ERR_DEVFILEOPEN;
		goto open_failed;
	}
	
	if (ioctl(fd, RAGGED_ZPU_STOP) < 0)
	{
		e = ZPU_ERR_COULDNOTSTOP;
		goto ioctl_failed;
	}
	
	close(fd);
	
	return ZPU_ERR_OK;
	
	ioctl_failed:
		printf("ioctl failed.\n");
		close(fd);
	open_failed:
		printf("open failed.\n");
	
	return e;
}

int zpu_start()
{
	int e;
	int fd;
	
	if ((fd = open("/dev/zpu", O_RDWR, S_IWUSR)) < 0)
	{
		e = ZPU_ERR_DEVFILEOPEN;
		goto open_failed;
	}
	
	if (ioctl(fd, RAGGED_ZPU_START) < 0)
	{
		e = ZPU_ERR_COULDNOTSTART;
		goto ioctl_failed;
	}
	
	close(fd);
	
	return ZPU_ERR_OK;
	
	ioctl_failed:
		close(fd);
	open_failed:
	
	return e;
}
