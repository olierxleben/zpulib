#include "include/zpu.h"

int main()
{
	if (zpu_stop() != 0) return -1;
	if (zpu_load_from_file("hw.hex") != 0) return -2;
	if (zpu_start() != 0) return -3;
}
