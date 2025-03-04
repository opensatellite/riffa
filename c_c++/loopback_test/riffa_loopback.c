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

int main(int argc, char** argv) {
	fpga_t * fpga;
	fpga_info_list info;
	int option;
	int i;
	int id;
	int chnl;
	size_t numWords;
	int sent = 0;
	int recvd;
	unsigned int * sendBuffer;
	unsigned int * recvBuffer;
	GET_TIME_INIT(3);

	if (argc < 2) {
		printf("Usage: %s <option>\n", argv[0]);
		return -1;
	}

	option = atoi(argv[1]);

	if (option == 0) {	// List FPGA info
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
	}
	else if (option == 1) { // Reset FPGA
		if (argc < 3) {
			printf("Usage: %s %d <fpga id>\n", argv[0], option);
			return -1;
		}

		id = atoi(argv[2]);

		// Get the device with id
		fpga = fpga_open(id);
		if (fpga == NULL) {
			printf("Could not get FPGA %d\n", id);
			return -1;
	    }

		// Reset
		fpga_reset(fpga);

		// Done with device
        	fpga_close(fpga);
	}
	else if (option == 2) { // Send data
		if (argc < 5) {
			printf("Usage: %s %d <fpga id> <chnl> <num words to transfer>\n", argv[0], option);
			return -1;
		}

		id = atoi(argv[2]);
		chnl = atoi(argv[3]);
		numWords = atoi(argv[4]);
		unsigned int firstval = 0;
                if(argc >= 6) firstval = atoi(argv[5]); 

		// Get the device with id
		fpga = fpga_open(id);
		if (fpga == NULL) {
			printf("Could not get FPGA %d\n", id);
			return -1;
	    	}

		// Malloc the arrays
		sendBuffer = (unsigned int *)malloc(numWords<<2);
		if (sendBuffer == NULL) {
			printf("Could not malloc memory for sendBuffer\n");
			fpga_close(fpga);
			return -1;
	    	}
		
		// Initialize the data
		for (unsigned int i = 0; i < numWords; i++) {
			sendBuffer[i] = i + firstval;
		}
		printf("offset: 0\n");
		for (i = 0; i < ((numWords > 16) ? 16 : numWords); i++) {
			printf("%08x ", sendBuffer[i]);
			if((i+1) % 8 == 0) printf("\n");
		}
		printf("\n");

		// Send the data
		GET_TIME_VAL(0);
		for(int i = 0; i < 1; i++) {
			sent += fpga_send(fpga, chnl, sendBuffer, numWords, 0, 1, 25000);
			printf("words sent: %d\n", sent);
		}
		GET_TIME_VAL(1);
		// Done with device
        	fpga_close(fpga);

		printf("time: %lf ms\n", TIME_VAL_TO_MS(1) - TIME_VAL_TO_MS(0));
		printf("speed: %lf MB/s\n", ((double)sent*4.0)/((double)TIME_VAL_TO_MS(1) - TIME_VAL_TO_MS(0))/1000.0);
	}

	else if (option == 3) { // Receive data
		if (argc < 5) {
			printf("Usage: %s %d <fpga id> <chnl> <num words to transfer> <first val>\n", argv[0], option);
			return -1;
		}

		id = atoi(argv[2]);
		chnl = atoi(argv[3]);
		numWords = atoi(argv[4]);

		// Get the device with id
		fpga = fpga_open(id);
		if (fpga == NULL) {
			printf("Could not get FPGA %d\n", id);
			return -1;
	    	}

		recvBuffer = (unsigned int *)malloc(numWords<<2);
		if (recvBuffer == NULL) {
			printf("Could not malloc memory for recvBuffer\n");
			fpga_close(fpga);
			return -1;
	    	}

		// Recv the data
for(int i = 0; i < 1; i++) {
		recvd = fpga_recv(fpga, chnl, recvBuffer, numWords, 25000);
		printf("words recv: %d\n", recvd);
}
		// Done with device
	        fpga_close(fpga);

		// Display some data
/*
		printf("offset: 0\n");
		for (i = 0; i < ((recvd > 16) ? 16 : recvd); i++) {
			printf("%08x ", recvBuffer[i]);
			if((i+1) % 8 == 0) printf("\n");
		}
		printf("\n");
		
		printf("offset: %d\n", (recvd-16 < 0) ? 0 : (recvd-16));
		for (i = ((recvd-16 < 0) ? 0 : (recvd-16)); i < recvd; i++) {
			printf("%08x ", recvBuffer[i]);
			if((i+1) % 8 == 0) printf("\n");
		}
		printf("\n");
*/

/*
		for (i = 0; i < recvd; i++) {
			printf("%08x ", recvBuffer[i]);
			if((i+1) % 8 == 0) printf("\n");
		}
		printf("\n");
*/

		printf("offset: %d\n", (recvd-16 < 0) ? 0 : (recvd-16));
		for (i = ((recvd-16 < 0) ? 0 : (recvd-16)); i < recvd; i++) {
			printf("%08x ", recvBuffer[i]);
			if((i+1) % 8 == 0) printf("\n");
		}
		printf("\n");
	}

	return 0;
}
