#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

typedef struct sigaction Sigaction;

// Waits a number of milliseconds
int waitSync(int millis) {
	// If not waiting for any time, return asap
	if (millis <= 0) { return 0; }

	struct timeval now, start;
	gettimeofday(&start, NULL);
	
	while (1) { // while (true)
		// Extremely wasteful (but accurate) way of waiting
		// Constantly poll for updated time,
		// and check to see if enough has elapsed.

		gettimeofday(&now, NULL);
		long int diff_usec = now.tv_usec - start.tv_usec;
		long int diff_sec = now.tv_sec - start.tv_sec;
		
		long int diff_msec = 
			diff_usec / 1000 // us -> ms
			+ diff_sec * 1000; // s -> ms

		if (diff_msec >= millis) { return 0; }
		
	}
}

// Signal handler methods. 
void handlerA(int signum) {
	printf("handlerA, got signal %d\n", signum);
}
void handlerB(int signum) {
	printf("handlerB, got signal %d\n", signum);
}
void handlerC(int signum) {
	printf("handlerC, got signal %d\n", signum);
}

// waits milliseconds, then sends a signal to a pid
void sendSig(int waitmillis, pid_t pid, int sig) {
	waitSync(waitmillis);
	printf("Child: Sending signal %d\n", sig);
	kill(pid, sig);
}


int main(void) {
	
	// Set up handler information and register signal handlers
	Sigaction actA;
	actA.sa_handler = handlerA;
	assert(sigaction(25, &actA, NULL ) == 0);

	Sigaction actB;
	actB.sa_handler = handlerB;
	assert(sigaction(12, &actB, NULL ) == 0);

	Sigaction actC;
	actC.sa_handler = handlerC;
	assert(sigaction(31, &actC, NULL ) == 0);

	pid_t pid = getpid();
	pid_t cpid = fork();
	
	if (cpid == 0) {
		// Child process:
		
		printf("hallo warld\n");
		sendSig(330, pid, 25);
		sendSig(330, pid, 12);
		sendSig(330, pid, 12);
		for (int i = 0; i < 3; i++) {
			sendSig(0, pid, 12);
		}

		sendSig(330, pid, 31);
		
		waitSync(100);
		printf("Child: Terminating parent with signal 9\n");
		kill(pid, 9);

	} else {
		// Parent process: 

		int cnt = 0;
		while (1) {
			waitSync(1);
			printf(".");
			if ( ! ( ++cnt % 32 ) ) { printf("\n"); }
		}
	}

	return 0;
}