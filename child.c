#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/auxv.h>
#include <sys/time.h>

typedef struct sigaction Sigaction;

// Waits a number of milliseconds
int waitSync(int millis) {
	// If not waiting for any time, return asap
	if (millis <= 0) { return 0; }

	struct timeval now, start;
	assert(gettimeofday(&start, NULL) == 0); 
	
	while (1) { // while (true)
		// Extremely wasteful (but accurate) way of waiting
		// Constantly poll for updated time,
		// and check to see if enough has elapsed.

		assert(gettimeofday(&now, NULL) == 0);
		long int diff_usec = now.tv_usec - start.tv_usec;
		long int diff_sec = now.tv_sec - start.tv_sec;
		
		long int diff_msec = 
			diff_usec / 1000 // us -> ms
			+ diff_sec * 1000; // s -> ms

		if (diff_msec >= millis) { return 0; }	
	}
}

// waits milliseconds, then sends a signal to a pid
void sendSig(int waitmillis, pid_t pid, int sig) {
	waitSync(waitmillis);
	assert(printf("Child: Sending signal %d to %d\n", sig, pid));
	assert(kill(pid, sig) == 0);
}

int main(int argc, char** argv) {
	if (argc >= 2) {
		pid_t pid = (pid_t) strtol(argv[1], NULL, 10);

		assert(printf("Child: Process starting, pid = %d\n", pid));
		sendSig(330, pid, 25);
		sendSig(330, pid, 30);
		//sendSig(330, pid, 30);
		for (int i = 0; i < 3; i++) {
			sendSig(0, pid, 25);
		}

		sendSig(330, pid, 31);
		
		waitSync(100);
		assert(printf("Child: Terminating parent with signal 9\n"));
		sendSig(0, pid, 30);
		sendSig(0, pid, 30);
		waitSync(330);
		assert(kill(pid, 9) == 0);
	} else {
		assert(printf("Need to be passed parent program PID to send signals to. Exiting."));
	}


	return 0;
}