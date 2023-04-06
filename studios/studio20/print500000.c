/******************************************************************************
* 
* print500000.c
*
* Prints numbers 1 to 500000 in order.
* 
* A simple program serving as a starting point
* for exploring several signal-handling concepts.
*
* Modified January 18, 2021 by Marion Sudvarg
* from a similar program used in previous semesters
* of CSE 422S at Washington University in St. Louis
* 
******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define size 500000

volatile int i;
volatile sig_atomic_t shouldterm = 0;

int called [size];

void sigaction_fn(int arg);

int main (int argc, char* argv[]){

	struct sigaction mysig;
	mysig.sa_handler = sigaction_fn;
	mysig.sa_flags = SA_RESTART;
	sigaction(SIGINT, &mysig, NULL);

        for(i = 0; i < size; i++){
                printf("i: %d\n", i);
		if (shouldterm)
			break;
        }

        for(i = 0; i < size; i++){
                if( called[i] == 1 )
                        printf("%d was possibly interrupted\n", i);
        }

        return 0;
}

void sigaction_fn(int arg) {
	char * msg = "Caught SIGINT";

	called[i] = 1;
	//shouldterm = 1;
	//write(STDOUT_FILENO, msg, strlen(msg));
	printf("Caught SIGINT");
	
}
