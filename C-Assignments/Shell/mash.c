/**
 * A basic shell implementation that includes the built-in commands pwd, cd, 
 * exit, and setenv. Also allows for IO redirection using <, >, and |. 
 *
 * @author  Franklin D. Worrell
 * @revised 21 September 2016, 23:45
 */ 

#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h> 

char path[PATH_MAX]; 		// buffer for current working directory. 

void sys_call_failed(const char* call); 
int find_redirects(char** token_array, int start, int end, int* index_array);
int handle_redirects(char** input, int end);
int find_pipes(char** token_array, int length, int* index_array); 

int main(void) {
    while (1) { // Infinite loop waits for user input. 
	    /* Get the current working directory and handle possible errors. */
	    if (getcwd(path, PATH_MAX) == NULL) {
		    sys_call_failed("getcwd"); 
	    } 

	    /* Print prompt for user input. */
	    printf("%s> ", path); 
	
	    /* Store user input for tokenizing. */
	    char* input = malloc(sysconf(_SC_ARG_MAX) + 1);
	    if (fgets(input, sysconf(_SC_ARG_MAX), stdin) == NULL) {
		    sys_call_failed("fgets");
	    } 
	    input[strcspn(input, "\r\n")] = '\0'; // Delete ending newline character. 
	
	    /* Being tokenizing input. */ 
	    int index = 0; 
	    char** tokenized_input = malloc(sizeof(char*) * (index + 1)); 
	
	    /* Capture first token and put into tokenized array. */
	    char* single_token = strtok(input, " "); // Break each token at spaces. 
	    if ((tokenized_input[index] = malloc(sizeof(char) * strlen(single_token))) 
	         == NULL) {
		    sys_call_failed("malloc"); 
	    } 
	    strcpy(tokenized_input[index], single_token);
	    index++; 
	
	    /* Iterate until there are no more tokens. */
	    while (single_token != NULL) {
		    single_token = strtok(NULL, " "); // Retrieve next token.
		    if (single_token != NULL) { // New string could be NULL. 
			    if ((tokenized_input = realloc(tokenized_input, 
			         sizeof(char*) * (index + 1))) == NULL) {
				    sys_call_failed("realloc line 63"); 
			    } 
			    if ((tokenized_input[index] = malloc(sizeof(char) * 
			         strlen(single_token))) == NULL) {
				    sys_call_failed("malloc"); 
			    }
			    strcpy(tokenized_input[index], single_token);
			    index++; 
		    }
	    } // Completed tokenizing input.  

	    int total_tokens = index; // The number  of tokens found in input. 
	
	    free(input); // Free memory of raw input.	    
	    
	    /* Parse tokenized input for pipes. */ 
		int* pipes_indices = malloc(sizeof(int)); 
		int number_pipes = find_pipes(tokenized_input, total_tokens, pipes_indices); 
		
		/* Execute pipes around commands. */ 
		int which_command = 0; 
		int command_start = 0; 
		int command_end; 
		int command_length; 
		int pipes[number_pipes][2]; 
		
	    do {
	        /* Catch the command and its arguments from the tokenized input. */ 
	        command_end = pipes_indices[which_command]; 
	        command_length = command_end - command_start; 
	        
	        char** com_and_args = malloc(sizeof(char*) * (command_length + 1)); 	
	        int ci; 
	        for (ci = 0; ci < command_length; ci++) {
	            com_and_args[ci] = malloc(sizeof(char) * strlen(tokenized_input[ci + command_start])); 
	            strcpy(com_and_args[ci], tokenized_input[ci + command_start]); 
	        } 
	        com_and_args[command_length] = NULL; 

            /* Set up the needed pipe. */ 
            if ((number_pipes > 0) && (which_command < number_pipes)) {
                if (pipe(pipes[which_command]) < 0) {
                    sys_call_failed("pipe line 111"); 
                } 
            } 
	
	        int pid; 
	        
	        /* Handle built-in commands that require special treatment. */ 
            if ((strcmp(com_and_args[0], "exit") == 0) ||
                (strcmp(com_and_args[0], "cd") == 0) || 
                (strcmp(com_and_args[0], "setenv") == 0)) {
                
                /* exit */ 
                if (strcmp(com_and_args[0], "exit") == 0) {
                    exit(0); 
                } 
                
                /* cd */ 
                else if (strcmp(com_and_args[0], "cd") == 0) {
                    if ((com_and_args[1] == NULL) || (strcmp(com_and_args[1], "~") == 0)) {
	                    if (chdir(getenv("HOME")) < 0) {
		                    sys_call_failed("chdir"); 
	                    }
                    } 
                    else {
	                    if (chdir(com_and_args[1]) < 0) {
		                    sys_call_failed("chdir"); 
	                    }
                    }
                } 
                
                /* setenv */ 
                else {
	                /* Tokenize the input in order to get variable name and value. */ 
	                char** var_and_val = malloc(sizeof(char*) * 2); 
	
	                /* Capture variable name and put into tokenized array. */
	                char* var_name = strtok(com_and_args[1], "="); // Break each token at spaces. 
	                if ((var_and_val[0] = malloc(sizeof(char) * strlen(var_name))) == NULL) {
		                sys_call_failed("malloc"); 
	                } 
	                strcpy(var_and_val[0], var_name);

                    /* Capture variable's new value and put into tokenized array. */ 
                    char* var_value = strtok(NULL, "="); // Retrieve next token. 
                    if ((var_and_val[1] = malloc(sizeof(char) * strlen(var_value))) == NULL) {
                        sys_call_failed("malloc"); 
                    } 
                    strcpy(var_and_val[1], var_value); 
	
	                if (setenv(var_name, var_value, 1) < 0) {
	                    sys_call_failed("setenv"); 
	                } 
		        } 
	        } 
	        
	        /* Command did not require special treatment. */ 
	        else {
	            if ((pid = fork()) < 0) {
	                sys_call_failed("fork"); 
	            } 
	
	            else if (pid == 0) { // Child process. 
	                /* There's an incoming pipe. */ 
	                if ((number_pipes > 0) && (which_command > 0)) {
	                    dup2(pipes[which_command - 1][0], 0);
	                } 
	                
	                /* There's an outgoing pipe.*/ 
	                if (which_command < number_pipes) {
	                    dup2(pipes[which_command][1], 1); 
	                } 
	                
	                /* Handle any redirects. */ 
	                int first_redirect = handle_redirects(com_and_args, command_length);
	                if (first_redirect < command_length) {
	                    com_and_args[first_redirect] = NULL; 
	                } 
	                
	                /* Handle built-in commands. */ 
	                if (strcmp(com_and_args[0], "pwd") == 0) {
	                    if (getcwd(path, PATH_MAX) == NULL) {
		                    sys_call_failed("getcwd"); 
	                    } 

	                    printf("%s\n", path);
                        exit(0); 	                
	                } 
	                
	                /* Run not built-in command. */ 	            
	                else { 
	                    if (execvp(com_and_args[0], com_and_args) < 0) {
	                        sys_call_failed("execvp"); 
	                    } 
	                } 
	            } 
	
	            else { // Parent process. 
	                /* There was an outgoing pipe.*/ 
	                if (which_command < number_pipes) {
	                    close(pipes[which_command][1]); 
	                } 
	                
	                int status; 
	                waitpid(pid, &status, 0); 
	            } 
            } // end running command in child	

   	        command_start = pipes_indices[which_command] + 1;             
            which_command++; 
        } while (command_start < total_tokens);         
	
	    /* Free memory used tokenizing input. */ 
	    int ti2; 
	    for (ti2 = 0; ti2 < total_tokens; ti2++) {
	        free(tokenized_input[ti2]); 
	    } 
	    free(tokenized_input); 
    } // end infinite while loop
} // end method main


/**
 * Prints the error message produced by the most recent failed 
 * system call. 
 * @param	the name of the system call that failed
 */ 
void sys_call_failed(const char* call) {
	perror(call); 
} // end function sys_call_failed


/**
 * Finds each redirect in subsection of tokenized user input. Saves the location
 * of each redirect in the array passed in as argument. 
 * @param	token_array		the tokeninzed input
 * @param   start           the first index of the token_array under consideration
 * @param   end             the index to stop searching the token_array
 * @param	index_array		array to store indices of pipes and redirects
 * @return	length of index_array after indices added
 */ 
int find_redirects(char** token_array, int start, int end, int* index_array) {
	int count = 0; // Number of < and > found in tokenized input. 
	int length = end - start; 
	
	/* Iterate through strings and find < and > instances. */ 
    int i; 
	for (i = 0; i < length; i++) {
		/* When a redirect is encountered, store index in array. */ 
		if ((strcmp(token_array[i], "<") == 0) || 
			(strcmp(token_array[i], ">") == 0)) {
			count++; 
			index_array[count - 1] = i; // Enter index into array of indices. 
		}
	} 

	index_array[count] = length; // Set the value to last index of input.
	return count; 
} // end function find_redirects


/**
 * Takes a section of the tokenized input, further parses it to locate < and >
 * instances, sets the I/O redirection appropriately, and then stores the 
 * command in the designated array of strings.  
 * @param	input		the tokenized input
 * @param	end			the index ending the relevant section of input
 * @return  the index of the first redirect
 */ 
int handle_redirects(char** input, int end) {
    int was_redirected = 0; 

	/* Parse tokenized input for redirects. */ 
	int redirs_indices[2]; 
	int number_redirs = find_redirects(input, 0, end, redirs_indices); 
	
	int which_redir = 0; 
	int next_redir_index = redirs_indices[which_redir]; 
	while (next_redir_index < end) {
		/* Redirect is <, so set input appropriately. */ 
		if (strcmp(input[next_redir_index], "<") == 0) {
			/* Open the file named in command for reading. */ 
			int fd_in; // File descriptor for opened file. 
			if ((fd_in = open(input[next_redir_index + 1], O_RDONLY)) < 0) {
				sys_call_failed("open");  
			} 
		
			/* Redirect stdin so that file is read. */ 
			dup2(fd_in, STDIN_FILENO);
		
			/* Close the file that was open. */ 
			close(fd_in); 
			
			was_redirected = 1; 
		} 
		
		/* Redirect is >, so set output appropriately. */ 
		else if (strcmp(input[next_redir_index], ">") == 0) {
			/* Open/create the file named in the command for writing. */ 
			int fd_out; 
			if ((fd_out = open(input[next_redir_index + 1], 
				 O_WRONLY | O_CREAT | O_TRUNC, 
				 S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH)) < 0) {
				sys_call_failed("open"); 
			} 
		
			/* Redirect stdout so that file is written. */ 
			dup2(fd_out, STDOUT_FILENO);
		
			/* Close the file that was open. */ 
			close(fd_out); 	
			
			was_redirected = 1; 	
		}
		
		which_redir++; 
		next_redir_index = redirs_indices[which_redir]; 
	} 
	
	return redirs_indices[0]; 
} // end function handle_redirects


/**
 * Finds each pipe in tokenized user input. Stores its index in the array
 * passed as argument. 
 * @param	token_array		the tokeninzed input
 * @param	length			the length of token_array
 * @param	index_array		array to store indices
 * @return	length of index_array after indices added
 */ 
int find_pipes(char** token_array, int length, int* index_array) {
	int count = 0; // Number of | found in tokenized input. 
	
	/* Iterate through strings and find | instances. */ 
    int i; 
	for (i = 0; i < length; i++) {
		/* When a pipe is encountered, store index in array. */ 
		if (strcmp(token_array[i], "|") == 0) {
			count++; 
			/* Expand the index array to hold another value. */ 
			if ((index_array = realloc(index_array, sizeof(int) * (count + 1))) 
			     == NULL) {
				sys_call_failed("realloc"); 
			} 
			index_array[count - 1] = i; // Enter index into array of indices. 
		}
	} 

	index_array[count] = length; 
	return count; 
} // end function find_pipes
