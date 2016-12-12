/* A basic implementation of the ls command with added functionality
 * that allows for (1) default listing of current directory if no 
 * location is specified by the user, (2) use of the -a flag, and 
 * (3) use of the -r flag. Additionally, output is listed in the same 
 * order as the system ls command. Makes use of library provided with 
 * Advanced Programming in the Unix Environment. 
 *
 * Assignment: Homework One - C Practice
 * Date: Wednesday, February 3, 2016
 * Author: Franklin D. Worrell
 */ 

#include "apue.h"
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

/* Structure for singly linked list to hold strings for processing */ 
struct node {
	char* 			name; 
	struct node* 	pNext; 
}; // end struct node 

/* Prototypes */ 
void add(struct node** ppHead, char* string);
void removeNode(struct node** ppNode, struct node** ppHead); 
void printList(struct node* pHead); 
void removeDotFiles(struct node** ppHead); 
void sortList(struct node** ppHead); 
char* findFlags(int argc, char* argv[]); 
char* findDirectory(int argc, char* argv[]); 
void reverseList(struct node** ppHead); 
char* trimDots(char* string);

/*
 * Parameters: argc		the number of arguments entered at command line
 * 				argv[]	the array of strings entered at command line */ 
int main(int argc, char* argv[]) {
	DIR			*dp;
	struct dirent		*dirp; 
	
	const char* currentDirectory = "."; 
	char* specifiedFlags = NULL;  
	char* specifiedDirectory = NULL; 
	
	/* User entered additional command line arguments. */ 
	if (argc != 1) {
		/* Parse the user's input to collect flags and directory. */
		specifiedFlags = findFlags(argc, argv);
		specifiedDirectory = findDirectory(argc, argv);
	}
	
	/* Create a linked list to hold file/directory names. */ 
	struct node* pListHead = NULL; 
	
	/* No directory specified by user. */ 
	if ((argc == 1) || (specifiedDirectory == NULL)) {
		if ( (dp = opendir(currentDirectory)) == NULL) {
			err_sys("can't open current directory"); 
		} 
		while ( (dirp = readdir(dp) ) != NULL) {
			add(&pListHead, dirp -> d_name); 
		} 
	} 
	
	/* User specified a directory. */ 
	else { 
		if ( (dp = opendir(specifiedDirectory))  == NULL )
			err_sys("can't open %s",argv[1]);
	
		while ( (dirp = readdir(dp) ) != NULL ) {
			add(&pListHead, dirp -> d_name); 
		} 
	} 
	
	/* Sort the list to match system ls output. */ 
	sortList(&pListHead); 
	
	/* Check to see if user entered a flag and respond appropriately. */ 
	/* No -a flag entered, so remove the "dot files". */ 
	if ((specifiedFlags == NULL) || (strchr(specifiedFlags, 'a') == NULL)) {
		removeDotFiles(&pListHead); 
	} 
	
	// -r flag entered, so reverse the list. 
	if ((specifiedFlags != NULL) && (strchr(specifiedFlags, 'r') != NULL)) {
			reverseList(&pListHead); 
	} 
	
	printList(pListHead); 

	closedir(dp);
	
	exit(0);
} // end main


/* Adds a new Node to a Linked list. 
 * Parameters: ppHead	a pointer to a pointer to the first Node of the list
 * 				string	the string the Node should contain */
void add(struct node** ppHead, char* string) {
	/* Build new Node and set its name field. */ 
	struct node* pNewNode = malloc(sizeof(struct node)); 
	pNewNode -> name = string; 
	
	/* Check to see if list was empty. 
	 * If it is, set the head and tail pointers to the new Node. */ 
	if (*ppHead == NULL) {
		*ppHead = pNewNode; 
		pNewNode -> pNext = NULL; 
	} 
	
	/* Otherwise, append the new Node to the end of the list. */ 
	else { 
		/* Perform the relevant reference shifting. */ 
		struct node* currentNode = *ppHead; 
		while (currentNode -> pNext != NULL) {
			currentNode = currentNode -> pNext; 
		} 
		currentNode -> pNext = pNewNode; 
		pNewNode -> pNext = NULL; 
	}
} // end function add 


/* Removes a Node from a linked list. Does NOT free the memory, because some methods
 * require removing the node from the list while retaining a reference to it. 
 * Parameters: ppNode	a pointer to a point to the Node to be removed
 * 				ppHead	a pointer to a pointer to the list's head */ 
void removeNode(struct node** ppNode, struct node** ppHead) {
	/* The Node to be removed is the head of the list. */ 
	if ( *ppNode == *ppHead) { 
		*ppHead = (*ppNode) -> pNext; 
	}
	
	/* The Node to be removed is in the middle of the list. */ 
	else {
		struct node* currentNode = *ppHead; 
		while (currentNode -> pNext != *ppNode) {
			currentNode = currentNode -> pNext; 
		} 
		currentNode -> pNext = (*ppNode) -> pNext; 
	}
} // end function remove


/* This rather crappy sort just creates a new linked list, looks for the 
 * smallest element in the original list, removes it from the original list, 
 * then adds it to the new list. Original head pointer is reset to new head. 
 * Parameters: ppHead		a pointer to a pointer to the list's first node */ 
void sortList(struct node** ppHead) {
	// Create a new node to serve at the head of the sorted list. 
	struct node* pNewHead = NULL; 
	
	// Create temp nodes to hold references. 
	struct node* pSmallestNode; 
	struct node* pCurrentNode; 
	
	// Start at the head of the list 
	while (*ppHead != NULL) {
		pSmallestNode = *ppHead; 
		pCurrentNode = (*ppHead) -> pNext; 
		
		// Get the string from the first node and remove any "."s. 
		char* smallestString = strdup(pSmallestNode -> name); 
		smallestString = trimDots(smallestString);  
		
		// Compare that string to the in each subsequent node. 
		while (pCurrentNode != NULL) {
			char* currentString = strdup(pCurrentNode -> name); 
			currentString = trimDots(currentString); 
			
			// If a later node has a "lower value" string, assign it to smallest. 
			if (strcasecmp(smallestString, currentString) > 0) {
				pSmallestNode = pCurrentNode; 
				smallestString = currentString; 
			}
			
			// Proceed to next node. 
			pCurrentNode = pCurrentNode -> pNext; 
		}
		// Remove the old smallest node from the list. 
		removeNode(&pSmallestNode, ppHead); 
		// Add the smallest node to the new list. 
		add(&pNewHead, pSmallestNode -> name); 
		// Set the smallest node to the new head of the list. 
		pSmallestNode = *ppHead; 
	} 
	
	// Change the list's head pointer to point at the head of the new, sorted list. 
	*ppHead = pNewHead; 
} // end function sortList


/* Reverses the order of the nodes in the linked list. 
 * Parameters:	ppHead		a pointer to a pointer to the list's first node */ 
void reverseList(struct node** ppHead) {
	// Temporary pointers to hold references to node and its neighbors. 
	struct node* pNodesPrevious = NULL; 
	struct node* pCurrentNode = *ppHead; 
	struct node* pNodesNext = NULL; 
	
	// Iterate through list and switch next references to point to previous node. 
	while (pCurrentNode != NULL) {
		pNodesNext = pCurrentNode -> pNext; 
		pCurrentNode -> pNext = pNodesPrevious; 
		pNodesPrevious = pCurrentNode; 
		pCurrentNode = pNodesNext; 
	}
	
	// Reset the list head pointer.
	*ppHead = pNodesPrevious; 
} // end function reverseList


/* Removes initial "dots" from a file name. Recursive call handles ".." cases. 
 * Parameters: string		the name the dots are being removed from 
 * Returns: 	the inputted string minus any initial "dots" */ 
char* trimDots(char* string) {
	char* tempTrim = malloc(sizeof(char) * (strlen(string) - 1)); 
	
	// Check if first character is '.'. 
	if (strncmp(string, ".", 1) == 0) {
		// Copy the string without the ".". 
		strcpy(tempTrim, string + 1); 
		// Recursive call to handle ".." case. 
		return (trimDots(tempTrim));
	} 
	
	return string; 
} // end function trimDots


/* Prints the string contained by each Node of a linked list in order.
 * Parameters: pHead		a pointer to the first Node of the linked list */ 
void printList(struct node* pHead) {
	/* Grab reference to first Node. */ 
	struct node* currentNode = pHead; 
	
	/* Iterate through the list, printing each Node's string. */ 
	while (currentNode != NULL) {
		printf("%s\n", currentNode -> name); 
		currentNode = currentNode -> pNext; 
	}  
} // end function printList


/* Removes all file names beginning with "."
 * Parameters:	ppHead		a pointer to a pointer the list's first node */ 
void removeDotFiles(struct node** ppHead) {
	/* Create a temporary linked list to hold Nodes without "dot files". */ 
	struct node* tempHead = NULL; 
	
	/* Capture a reference to the first Node. */ 
	struct node* currentNode = *ppHead; 
	
	/* Iterate through the original list, and add duplicate Nodes to new one. */ 
	while (currentNode != NULL) {
		if (strncmp(currentNode -> name, ".", 1) != 0) {
			add(&tempHead, currentNode -> name); 
		} 
		
		currentNode = currentNode -> pNext; 
	}
	
	/* Reset the head and tail of the original list to the new one's. */ 
	*ppHead = tempHead; 
} // end function removeDotFiles


/* Parses user input to isolate flags and return them in a single string; returns
 * NULL if none entered.
 * Parameters: argc		the number of command line arguments inputted
 *             argv		the array of strings of the command line arguments 
 * Returns: 	a string containing all of the inputted flags */ 
char* findFlags(int argc, char* argv[]) {
	char* flags = NULL; 
	
	/* Checks second string in argv to see if it was the flags. */ 
	if (strncmp(argv[1], "-", 1) == 0) {
		flags = argv[1]; 
	} 
	
	/* If there was a third argument, looks for flags there. */ 
	else if ((argc == 3) && (strncmp(argv[2], "-", 1) == 0)) {
		flags = argv[2]; 
	} 
	
	return flags; 
} // end function findFlags


/* Parses user input to isolate any entered directory; returns NULL if none 
 * entered.
 * Parameters: argc		the number of command line arguments inputted
 *             argv		the array of strings of the command line arguments 
 * Returns: 	a string containing the inputted directory */ 
char* findDirectory(int argc, char* argv[]) {
	char* directory = NULL; 
	
	/* Make sure the string wasn't the flags. */ 
	if (strncmp(argv[1], "-", 1) != 0) {
		directory = argv[1]; 
	} 
	
	/* Takes third item, if second was flags. */ 
	else if (argc == 3) {
		directory = argv[2]; 
	} 
	
	return directory; 
} // end function findDirectory