#include "riffa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int ret;

    if (argc == 1) {
	printf("usage: %s addr [val]\n", argv[0]);
    	return -1;
    }    

    fpga_t *fpga = fpga_open(0);
    if(!fpga) return -1;

    if(argc == 3) {
        unsigned int addr = strtoul(argv[1], NULL, 0);
        unsigned int val = strtoul(argv[2], NULL, 0);
	    fpga_axi_write(fpga, addr, val);
        printf("w %08x %08x\n", addr, val);
    }
    
    if(argc == 2) {
        unsigned int addr = strtoul(argv[1], NULL, 0);
	    unsigned int val;
	    fpga_axi_read(fpga, addr, &val);
        printf("r %08x %08x\n", addr, val);
    }

    return 0;
}
