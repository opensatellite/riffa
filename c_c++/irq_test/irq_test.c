#include <stdio.h>       
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

//NOTE: sudo setcap 'cap_sys_nice=eip' irq_test
void enable_rtsched() {
    int result;
    int pid = 0;  // this process

    //set scheduler
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    result = sched_setscheduler(pid, SCHED_FIFO, &param);
    if(result < 0) goto error;

    result = setpriority(PRIO_PROCESS, 0, -20);
    if(result < 0) goto error;
    
    return;

error:
    if(result == -EPERM) {
        printf("enable_rtsched: permission denied\n");
    } else {
        printf("enable_rtsched failed %d\n", result);
    }
}

int runflag = 1;
//void (*orig_handler)(int) = NULL;
void sigint_handler(int s) {
    runflag = 0;
}

int main(int argc, char *argv[]) 
{
    if(argc < 2) {
        printf("usage: %s /dev/riffa[0-m]_events[0-n]\n", argv[0]);
        return 0;
    }
    
    enable_rtsched();

    int irq = open(argv[1], O_RDONLY);
    if(irq < 0) {
        printf("%s open failed %d\n", argv[1], irq);
        exit(irq);
    }

    FILE* log = fopen("log.txt", "w");
    if(!log) {
        printf("log.txt open failed\n");
    }

    //orig_handler = signal(SIGINT, sigint_handler);
    signal(SIGINT, sigint_handler);

    int totalcount = 0;
    while(runflag) {
        //struct pollfd p = {.fd = irq, .events = POLLIN};
        //if(poll(&p, 1, -1)) {
            struct timespec tv;
            uint32_t count;
            int ret = read(irq, &count, 4);
            clock_gettime(CLOCK_MONOTONIC, &tv);
            double t = tv.tv_sec * 1e6 + tv.tv_nsec / 1e3;
            if(ret <= 0) {
                if(ret == -EINTR) {
                    printf("stopping...\n");
                    break;		
                } else {
                    printf("[%.3lf]read failed %d\n", t, ret);
                }
            } else {
                printf("[%.3lf]count = %d\n", t, count);
                totalcount += count;
                if(log) {
                    fprintf(log, "%lf\n", t);
                }
            }
        //}
    }

    printf("totalcount = %d\n", totalcount);

    close(irq);
    fclose(log);
    return 0;
}
