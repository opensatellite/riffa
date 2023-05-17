#!/bin/bash

LEN=64
#MAXLEN=1073741824
MAXLEN=16777216

#LEN=8192
#MAXLEN=32768

OFFSET=0
TXTOTAL=0
RXTOTAL=0

# to run the test, start rx first

if [ "$1" == "tx" ];
then
	while [ $LEN -le $MAXLEN ] 
	do
		echo "LEN=$LEN, OFFSET=$OFFSET"
		./riffa_loopback 2 0 0 $LEN $OFFSET
		OFFSET=$(($OFFSET + $LEN))
		TXTOTAL=$(($TXTOTAL + $LEN))
                LEN=$(($LEN * 2))
	done
	printf 'TX EXPECTED LAST BLOCK FIRST WORD=%08x\n' $(($TXTOTAL - $LEN / 2))
        printf 'PLEASE CHECK THIS MANUALLY\n'
elif [ "$1" == "rx" ]; 
then
	while [ $LEN -le $MAXLEN ]  
	do
		echo "LEN=$LEN"
		./riffa_loopback 3 0 0 $LEN 
		RXTOTAL=$(($RXTOTAL + $LEN))
                LEN=$(($LEN * 2)) 
	done
	printf 'RX EXPECTED LAST BLOCK LAST WORD=%08x\n' $(($RXTOTAL - 1))
        printf 'PLEASE CHECK THIS MANUALLY\n'
elif [ "$1" == "rst" ];
then
	./riffa_loopback 1 0
else
	echo "$0 [tx:rx:reset]"
fi

