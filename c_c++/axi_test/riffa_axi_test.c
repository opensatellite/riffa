// ----------------------------------------------------------------------
// Copyright (c) 2016, The Regents of the University of California All
// rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of The Regents of the University of California
//       nor the names of its contributors may be used to endorse or
//       promote products derived from this software without specific
//       prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REGENTS OF THE
// UNIVERSITY OF CALIFORNIA BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
// ----------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "timer.h"
#include "riffa.h"

#define NREGS 32

void dump_all(fpga_t *fpga)
{
	int i;
	unsigned int val;
	int ret;
	int writable;
	// Dump all registers
	for (i = 0; i < NREGS; i ++) {
		ret = fpga_axi_read(fpga, i*4, &val);
		writable = ((i == 8) | (i == 10) | (i == 11) | (i == 14) | (i == 16)) ? 1 : 0;
		printf("addr: %08x, val: %08x, ret %d %c\n", i, val, ret, writable ? '*' : ' ');
	}
	printf("\n");
}

int main(int argc, char** argv) {
	fpga_t * fpga;
	fpga_info_list info;
	int i;
	int id = 0;
	unsigned int val[NREGS];
	int ret;
	GET_TIME_INIT(3);

	// Populate the fpga_info_list struct
	if (fpga_list(&info) != 0) {
		printf("Error populating fpga_info_list\n");
		return -1;
	}
	printf("Number of devices: %d\n", info.num_fpgas);
	for (i = 0; i < info.num_fpgas; i++) {
		printf("%d: id:%d\n", i, info.id[i]);
		printf("%d: num_chnls:%d\n", i, info.num_chnls[i]);
		printf("%d: name:%s\n", i, info.name[i]);
		printf("%d: vendor id:%04X\n", i, info.vendor_id[i]);
		printf("%d: device id:%04X\n", i, info.device_id[i]);
	}
	printf("\n");
	
	fpga = fpga_open(id);
	if(!fpga) {
		printf("fpga %d open failed\n", id);
		return -1;
	}
	
	dump_all(fpga);

	GET_TIME_VAL(0);
	for (i = 0; i < NREGS; i ++) {
		ret = fpga_axi_read(fpga, i*4, val + i);
	}
	GET_TIME_VAL(1);
	for(i = 0; i < NREGS; i++) {
		fpga_axi_write(fpga, i*4,  val[i] + 1);
	}
	GET_TIME_VAL(2);
	dump_all(fpga);

	double t_read = TIME_VAL_TO_MS(1) - TIME_VAL_TO_MS(0);
	double t_write = TIME_VAL_TO_MS(2) - TIME_VAL_TO_MS(1);
	printf("Read %d took %g ms, Write %d took %g ms\n", NREGS, t_read, NREGS, t_write);
	
	fpga_close(fpga);

	return 0;
}
