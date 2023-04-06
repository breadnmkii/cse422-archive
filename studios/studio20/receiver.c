/******************************************************************************
* 
* receiver.c
*
* A signal receiver program for the Signals studio
* in CSE 422S at Washington University in St. Louis
* 
* Written by Marion Sudvarg
* Last modified January 18, 2021
* 
******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <sys/time.h>

volatile sig_atomic_t done = 0;
volatile int count = 0;

void sig_handler( int signum, siginfo_t * si, void * ucontext ){

    if (si->si_value.sival_int == 0) {
        count++;
    }

    if (si->si_value.sival_int == 1) {
        done = 1;
    }
}

int main (int argc, char* argv[]){

    struct sigaction ss;

    ss.sa_sigaction = sig_handler;
    ss.sa_flags = SA_RESTART | SA_SIGINFO;

    sigaction( SIGRTMIN, &ss, NULL );
    // sigaction( SIGRTMIN, &ss, NULL );

    printf("pid: %d\n", getpid());

    while (!done) {}

    printf("SIGRTMIN received %d times\n", count);

    return 0;
}

