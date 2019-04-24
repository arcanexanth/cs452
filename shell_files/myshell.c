/*
 * This code implements a simple shell program
 * It supports the internal shell command "exit", 
 * backgrounding processes with "&", input redirection
 * with "<" and output redirection with ">".
 * However, this is not complete.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

//#define  DBUG

extern char **getaline();

/*global variables*/

char **args; 
char *output_filename;
char *input_filename;
char *append_filename;

/*
 * Handle exit signals from child processes
 */
void sig_handler(int signum) {
	int status, dead_pid;
	if(signum == SIGINT){
		signal(SIGINT, SIG_IGN);
		signal(SIGINT, sig_handler);
	} else if(signum == SIGCHLD){
		while((dead_pid = waitpid(-1, &status, WNOHANG))>0){
			if(WIFEXITED(status)) {
				#ifdef DBUG
				printf("\nPID %d EXITED NORMALLY\n", dead_pid);
				#endif
			} else if(WIFSIGNALED(status)) {
				#ifdef DBUG
				printf("\nPID %d SIGNALED TO EXIT\n", dead_pid);
				#endif
			}
		}
	}
}
/*
 * The main shell function
 */ 
main() {
	int i;
	int result;
	int block;
	int output;
	int input;
	int append; 

    int shellTTY = STDIN_FILENO;
    int shellPGID;
    int currPID;
    int currPGID;
	
    if(isatty(shellTTY)){
        while(tcgetpgrp(shellTTY) != (shellPGID = getpgrp()))
                kill(shellPGID, SIGTTIN);
    }
    /*set all signals*/
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, sig_handler);

    /*SET IDS*/
    currPID = currPGID = getpid();
    setpgid(currPID, currPGID);
    tcsetpgrp(shellTTY, currPGID);
        
	// Loop forever
	while(1) {
		// Print out the prompt and get the input
		printf("GOAT->");
		args = getaline();

		// No input, continue
		if(args[0] == NULL)
			continue;
        
        for(i=0; args[i] !=NULL; i++);
		// Check for internal shell commands, such as exit
		if(internal_command(args))
			continue;

		// Check for an ampersand
		block = (ampersand(args) == 0);

		// Check for redirected input
		input = redirect_input(args, &input_filename);

		switch(input) {
			case -1:
				printf("Syntax error!\n");
				continue;
				break;
			case 0:
				break;
			case 1:
				printf("Redirecting input from: %s\n", input_filename);
				break;
		}

		// Check for appended output
		append = append_output(args, &append_filename);

		switch(append) {
			case -1:
				printf("Syntax error!\n");
				continue;
				break;
			case 0:
				break;
			case 1:
				printf("Appending output to: %s\n", append_filename);
				break;
		}

				// Check for redirected output
		output = redirect_output(args, &output_filename);

		switch(output) {
			case -1:
				printf("Syntax error!\n");
				continue;
				break;
			case 0:
				break;
			case 1:
				printf("Redirecting output to: %s\n", output_filename);
				break;
		}
		processSymbols(args, shellTTY, currPGID, i, block, input, input_filename, output, output_filename, append, append_filename);
				// Do the command
				/*    do_command(args, block, 
					  input, input_filename, 
					  output, output_filename, append, append_filename);
					  */
	}
}

/*
 * Check for ampersand as the last argument
 */
int ampersand(char **args) {
	int i;

	for(i = 1; args[i] != NULL; i++) ;

	if(args[i-1][0] == '&') {
		free(args[i-1]);
		args[i-1] = NULL;
		return 1;
	} else {
		return 0;
	}

	return 0;
}

/* 
 * Check for internal commands
 * Returns true if there is more to do, false otherwise 
 */
int internal_command(char **arg) {
    int i;
	if(strcmp(args[0], "exit") == 0) {
		exit(0);
	}

	return 0;
}

/* 
 * Do the command
 */
int do_command(char **args, int shellTTY, int currPGID, int block,
				int input, char *input_filename,
				int output, char *output_filename, int append, char *append_filename) {

	int result;
	pid_t child_id;
	int status;

	int numPipes = 0;
	int i;

	//array to keep track of index of cmd after pipe
	//example: ls -la | grep total
	//cmdIndex would be [0, 3]
	int cmdIndex[10]; //10 because max amount of args from lex
	cmdIndex[0] = 0;

	//count number of pipes
	int tmp = 1;
	for(i = 0; args[i] != NULL; i++) {
		if(args[i][0] == '|') {
			args[i] = NULL; //cheap trick to not need second array for current cmd. execvp stops when reaching null
			cmdIndex[tmp] = i + 1; //index right after pipe
			tmp++;
			numPipes++;
		}
	}

	//array of file descriptors
	//example: 1 pipe = [read0, write0]
	//2 pipe = [read0, write0, read1, write1]
	//etc
	int fds[2 * numPipes];
	
	//set up pipes needed
	for(i = 0; i < numPipes; i++) {
		pipe(fds + i*2);
	}
	for(i = 0; args[i] != NULL; i++) {
		if(args[i][0] == '|') {
			
		}
	}

	int currCmd;
	int currCmdFD = 0;
	for(i = 0; i < numPipes + 1; i++) {
		currCmd = cmdIndex[i];
		// Fork the child process
		child_id = fork();

		// Check for errors in fork()
		switch(child_id) {
			case EAGAIN:
				perror("Error EAGAIN: ");
				return;
			case ENOMEM:
				perror("Error ENOMEM: ");
				return;
		}

		if(child_id == 0) {
	        #ifdef DBUG
                fprintf(stdout, "inside child_id==0, child_id is %d", child_id);
            #endif

            setpgid(child_id, child_id);
            if(block)
                    tcsetpgrp(shellTTY, getpid());

            /*child signals setup*/
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGCHLD, sig_handler);




            // Set up redirection in the child process
			if(input)
				freopen(input_filename, "r", stdin);
			if(output)
				freopen(output_filename, "w+", stdout);
			if(append)
				freopen(append_filename, "a", stdout);

			//if not last command
			if(i < numPipes) {
				//dup2 with cmd0 write, cmd1 read
				//curr cmd write -> next cmd read
				dup2(fds[currCmdFD + 1], 1);
			}

			//if not first command
			if(currCmdFD != 0) {
				//dup2 with prevCmdRead, prevCmdWrite
				//prev cmd read -> prev cmd write
				dup2(fds[currCmdFD - 2], 0);
			}
				
			//close fds
			for(i = 0; i < 2 * numPipes; i++) {
				close(fds[i]);
			}

			// Execute the command
            if((result = execvp(args[currCmd], args + currCmd))<0){
                perror("ERROR EXECUTING COMMAND!\n");
                _exit(-1); 
            }
			_exit(0);
		}

		//move to next cmd read file descriptor
		currCmdFD += 2;
	}
	//close remaining open fds
	for(i = 0; i < 2 * numPipes; i++) {
		close(fds[i]);
	}
	// Wait for the child process to complete, if necessary
    if(block){
        tcsetpgrp(shellTTY, child_id); //Give control to the child
        #ifdef DBUG
            fprintf(stdout, "Waiting for child, pid = %d\n", child_id);
        #endif
            result = waitpid(child_id, &status, WUNTRACED);
            tcsetpgrp(shellTTY, getpid()); //Return control to shell executable
            return WEXITSTATUS(status);
    }
}



int processSymbols(char **args, int shellTTY, int currPGID, int size, int block,
				int input, char *input_filename,
				int output, char *output_filename, int append, char *append_filename) {

	char **newArgs = malloc((size*sizeof(char*))+1);
	int i;
    
    if((*newArgs = malloc((size*sizeof(char*))+1))==NULL)
            perror("ERROR MALLOC INTERNAL NEWARGS");

	
    int rc;
	int newCurrIndex = 0;
	int skip = 0;
    char *prevCmd;

	//clear out newArgs
	for(i = 0; newArgs[i]; i++) {
		newArgs[i] = NULL;
	}

	for(i = 0; args[i] != NULL; i++) {
		if(args[i][0] == '(') {
            if(((strcmp(prevCmd, "&&")==0) && skip)||((strcmp(prevCmd, "||")==0) && skip)){
                #ifdef DBUG
                    fprintf(stdout, "INSIDE SHIFT AHEAD TEST");
                #endif
                    i = shiftahead(args, i);
            }else{
			    clearArray(newArgs);
			    newCurrIndex = 0;
            }
		} else if(args[i][0] == ')') {
			if(!skip && newArgs[0] != NULL) {
				rc = do_command(newArgs, shellTTY, currPGID, block, input, input_filename, output, output_filename, append, append_filename);
			}

			clearArray(newArgs);
			newCurrIndex = 0;
		} else if(args[i][0] == ';') {
			if(!skip && newArgs[0] != NULL) {
				rc = do_command(newArgs, shellTTY, currPGID, block, input, input_filename, output, output_filename, append, append_filename);
			} else {
				skip = 0;
			}

			clearArray(newArgs);
			newCurrIndex = 0;
		
		} else if(args[i][0] == '&' && args[i+1][0] == '&') {

			if(!skip && newArgs[0] != NULL) {
                prevCmd = "&&";
				rc = do_command(newArgs, shellTTY, currPGID, block, input, input_filename, output, output_filename, append, append_filename);
			} 

			clearArray(newArgs);
			newCurrIndex = 0;
			i++; //move past &&
			#ifdef DBUG
			printf("rc: %d\n", rc);
			#endif
			//skip next command if first command fails
			if(rc != 0) {
				skip = 1;
			} else {
				skip = 0;
			}

		} else if(args[i][0] == '|' && args[i+1][0] == '|') {
			if(!skip && newArgs[0] != NULL) {
                prevCmd = "||";
				rc = do_command(newArgs, shellTTY, currPGID, block, input, input_filename, output, output_filename, append, append_filename);
			}

			clearArray(newArgs);
			newCurrIndex = 0;
			i++; //move past ||
			#ifdef DBUG
			printf("rc: %d\n", rc);
			#endif
			//run second command if first command fails
			if(rc != 0) {
				skip = 0;
			} else {
				skip = 1;
			}


		} else {
		//not special symbol so add to newArgs
		newArgs[newCurrIndex] = args[i];
		newCurrIndex++;
			if(args[i+1] == NULL) {
				//End of args line from user
				if(!skip && newArgs[0] != NULL) {
					do_command(newArgs, shellTTY, currPGID, block, input, input_filename, output, output_filename, append, append_filename);
					#ifdef DBUG
					printf("rc: %d\n", rc);
					#endif
				}
			}
		}

	}
    for(i = 0; newArgs[i] !=NULL; i++){
        free(newArgs[i]);
    }
    free(newArgs);
	return 0;
}

clearArray(char **newArgs) {
	int i;
	for(i = 0; newArgs[i] != NULL; i++) {
		newArgs[i] = NULL;
	}
}

int shiftahead(char **args, int i){
    while(args[i][0] != ')'){
        i++;
    }
    return i+1;
}

/*
 * Check for input redirection
 */
int redirect_input(char **args, char **input_filename) {
	int i;
	int j;
	for(i = 0; args[i] != NULL; i++) {

		// Look for the <
		if(args[i][0] == '<') {
			free(args[i]);
			// Read the filename
			if(args[i+1] != NULL) {
				*input_filename = args[i+1];
			} else {
			return -1;
			}

			// Adjust the rest of the arguments in the array
			for(j = i; args[j-1] != NULL; j++) {
				args[j] = args[j+2];
			}

			return 1;
		}
	}
	return 0;
}

/*
 * Check for output redirection
 */
int redirect_output(char **args, char **output_filename) {
	int i;
	int j;

	for(i = 0; args[i] != NULL; i++) {

	// Look for the >
		if(args[i][0] == '>') {
			free(args[i]);

			// Get the filename 
			if(args[i+1] != NULL) {
				*output_filename = args[i+1];
			} else {
			return -1;
			}

			// Adjust the rest of the arguments in the array
			for(j = i; args[j-1] != NULL; j++) {
				args[j] = args[j+2];
			}

			return 1;
			}
		}
	return 0;
}

/*
 * Check for appended output
 */
int append_output(char **args, char **append_filename) {
		//basically exactly like redirect_output
	int i;
	int j;
	for(i = 0; args[i] != NULL; i++) {
		// Look for the >>
		if(args[i][0] == '>' && args[i+1][0] == '>') {
			free(args[i]);
			free(args[i+1]);
			// Get the filename 
			if(args[i+2] != NULL) {
				*append_filename = args[i+2];
			} else {
				return -1;
			}
			// Adjust the rest of the arguments in the array
			for(j = i; args[j-1] != NULL; j++) {
				args[j] = args[j+3];
			}
		return 1;
		}
	}
	return 0;
}
