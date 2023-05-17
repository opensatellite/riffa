#!/bin/bash

#LEN=64
#MAXLEN=1073741824

#LEN=1073741824  # 4GB	
LEN=67108864  # 256MB	
#LEN=1048576 # 4MB
NLOOP=0
MAXLOOP=1

TXTOTAL=0
RXTOTAL=0

# for speed test, disable axi speed limiter
echo "read speed limit"
./riffa_axi 0
echo "write speed limit: maximum"
./riffa_axi 0 1 

# to run the test, start tx first

if [ "$1" == "tx" ];
then
	while [ $NLOOP -lt $MAXLOOP ];
	do
		let IDX=$NLOOP+1
		echo "TEST=$IDX/$MAXLOOP, LEN=$LEN"
		./riffa_loopback 2 0 0 $LEN 
                let NLOOP+=1
		TXTOTAL=$(($TXTOTAL + $LEN))
	done
elif [ "$1" == "rx" ]; 
then
	while [ $NLOOP -lt $MAXLOOP ]; 
	do
		let IDX=$NLOOP+1
		echo "TEST=$IDX/$MAXLOOP, LEN=$LEN"
		./riffa_loopback 3 0 0 $LEN 
                let NLOOP+=1
		RXTOTAL=$(($RXTOTAL + $LEN))
	done
else
	echo "$0 [tx:rx]"
fi

