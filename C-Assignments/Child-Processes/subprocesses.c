/* Examines the relationship between parent, child, and grandchild
 * processes in terms of their IDs and parent IDs. 
 * 
 * Assignment: Homework 3 - Processes and Subprocesses
 * Date Due: Wednesday, April 27, 2016
 * Revision Date: Monday, April 25, 2016
 * Author: Franklin D. Worrell
 * Revision Date: April 23, 2016
 */ 

/* Required header files */ 
#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 

/* Prototypes */ 
void grandchildFinishesFirst(void); 
void childFinishesFirst(void); 
void parentFinishesFirst(void); 
void printParentProcessInfo(void); 
void printChildProcessInfo(void); 
void printGrandchildProcessInfo(void); 
void printProcessInfo(void); 
void spawnOrderedProcesses(int, int, int);

/* Takes the command line integer argument provided by the user and orients the
 * behavior of the three processes accordingly.
 * Parameters: 	argc	the number of arguments entered at command line
 *            	argv	the array of strings entered at command line */ 
int main(int argc, char *argv[]){
	// User entered too many or too few command line arguments. 
	if (argc != 2) {
		printf("User provided incorrect number of command line arguments.\n"); 
	} 

	// User entered correct number of command line arguments. 
	else {
		// The grandchild process should finish before the child and the parent.
		if (*argv[1] == '1') {
			grandchildFinishesFirst(); 
			// Put this process to sleep for long enough to allow all processes to terminate
			sleep(10); 
		} 

		// The child process should finish before the parent and the grandchild.
		else if (*argv[1] == '2') {
			childFinishesFirst(); 
			// Put this process to sleep for long enough to allow all processes to terminate
			sleep(10); 
		} 

		// The parent process should finish before the child and the grandchild.
		else if (*argv[1] == '3') {
			parentFinishesFirst(); 
			// Put this process to sleep for long enough to allow all processes to terminate
			sleep(10); 
		} 

		// User entered invalid command line argument. 
		else {
			printf("User entered an unrecognized command line argument.\n"); 
		} 
	} 
	exit(0); 
} // end main


/* Spawns processes that terminate in the following order: grandchild, child,
 * and parent. Prints the stats on each process before and after each sleeps. */ 
void grandchildFinishesFirst(void) {
	// Print info about the option the user selected. 
	printf("User entered 1. Order of termination: \n"); 
	printf("	1. Grandchild process; \n"); 
	printf("	2. Child process; and \n"); 
	printf("	3. Parent process. \n\n"); 
	printf("Before processes sleep: \n\n"); 

	// Create processes, put them to sleep, print results. 
	spawnOrderedProcesses(9, 6, 3); 
} // end function grandchildFinishesFirst


/* Spawns processes that terminate in the following order: child, parent,
 * and grandchild. Prints the stats on each process before and after each 
 * sleeps. */ 
void childFinishesFirst(void) {
	// Print info about the option the user selected. 
	printf("User entered 2. Order of termination: \n"); 
	printf("	1. Child process; \n"); 
	printf("	2. Parent process; and \n"); 
	printf("	3. Grandchild process. \n\n"); 

	// Create processes, put them to sleep, print results. 
	spawnOrderedProcesses(6, 3, 9); 
} // end function childFinishesFirst


/* Spawns processes that terminate in the following order: parent, child,
 * and grandchild. Prints the stats on each process before and after each 
 * sleeps. */ 
void parentFinishesFirst(void) {
	// Print info about the option the user selected. 
	printf("User entered 3. Order of termination: \n"); 
	printf("	1. Parent process; \n"); 
	printf("	2. Child process; and \n"); 
	printf("	3. Grandchild process. \n\n"); 

	// Create processes, put them to sleep, print results. 
	spawnOrderedProcesses(3, 6, 9); 
} // end function parentFinishesFirst


/* Prints the required IDs for the parent process. */ 
void printParentProcessInfo(void) {
	printf("I am the parent process.\n"); 
	printProcessInfo(); 
} // end function printParentProcessInfo 


/* Prints the required IDs for the child process. */ 
void printChildProcessInfo(void) {
	printf("I am the child process.\n"); 
	printProcessInfo(); 
} // end function printChildProcessInfo


/* Prints the required IDs for the grandchild process. */ 
void printGrandchildProcessInfo(void) {
	printf("I am the grandchild process.\n"); 
	printProcessInfo(); 
} // end function printGrandchildProcessInfo


/* Prints the process ID, parent process ID, and group ID of the currently
 * running process. */ 
void printProcessInfo(void) {
	printf("	My pid is: 	%d\n", getpid()); 
	printf("	My ppid is:	%d\n", getppid()); 
	printf("	My gid is:	%d\n", getgid()); 
	printf("	My sid is: 	%d\n", getsid(getpid())); 
} // end function printProcessInfo


/* Spawns three processes and assigns each an amount of time to sleep. 
 * Parameters: 	pWait	the parent process's sleep time
 *             	cWait	the child process's sleep time
 *             	gWait	the grandchild process's sleep time */ 
void spawnOrderedProcesses(int pWait, int cWait, int gWait){
	// Declare variables to store IDs of child and grandchild.
	pid_t parentID; 
	pid_t childID;
	pid_t grandchildID; 

	// Spawn a new process to act as parent. 
	if ((parentID = fork()) < 0) {
		printf("Error creating child process.\n"); 
	} 

	// Inside parent process. 
	else if (parentID == 0) {
		// Print parent's info before children created. 
		printParentProcessInfo(); 

		// Create child process and check for errors.  
		if ((childID = fork()) < 0) {
			printf("Error creating child process.\n"); 
		} 

		// Inside child process. 
		else if (childID == 0) {
			// Print child's info before grandchild created. 
			printChildProcessInfo(); 

			// Create grandchild process and check for errors. 
			if ((grandchildID = fork()) < 0) {
				printf("Error creating grandchild process.\n"); 
			} 
		
			// Inside grandchild process. 
			else if (grandchildID == 0) {
				// Print grandchild's info before performing an action.
				printGrandchildProcessInfo(); 
				// Print notice that sleep and terminations have begun. 
				printf("\nAt termination of each process: \n\n"); 
				// Grandchild terminates first, sleeps the least. 
				sleep(gWait);
				// Print grandchild's info after sleeping, before exiting. 
				printGrandchildProcessInfo();  
				exit(0); 
			} // Leaving grandchild process

			// Child sleeps a middle amount to terminate second.
			sleep(cWait); 
			// Print the child's info after sleeping, before exiting. 
			printChildProcessInfo(); 
			exit(0); 
		} // Leaving child process. 

		// Parent sleeps the longest to guarantee proper termination order.
		sleep(pWait); 
		// Print the parent's info after sleeping, before exiting. 
		printParentProcessInfo(); 
		exit(0); 
	} // Leaving parent process. 
} // end function spawnOrderedProcesses
