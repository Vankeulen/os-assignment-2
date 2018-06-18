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


int main(void) {
	
	// Set up handler information and register signal handlers
	struct sigaction actA;
	actA.sa_handler = handlerA;
	assert(sigaction(25, &actA, NULL ) == 0);

	struct sigaction actB;
	actB.sa_handler = handlerB;
	assert(sigaction(30, &actB, NULL ) == 0);

	struct sigaction actC;
	actC.sa_handler = handlerC;
	assert(sigaction(31, &actC, NULL ) == 0);

	pid_t pid = getpid();
	pid_t cpid = fork();
	
	if (cpid == 0) {
		// Child process:
		static char buffer[256];
		assert(sprintf(buffer, "./child %d", pid) > 0);

		execl("/bin/sh", "sh", "-c", buffer, (char*)0);
		perror("execl");
	} else {
		// Parent process: 

		int cnt = 0;
		while (1) {
			waitSync(1);
			assert(printf("."));
			if ( ! ( ++cnt % 32 ) ) { assert(printf("\n")); }
		}
	}

	return 0;
}
