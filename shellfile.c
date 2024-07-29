/*
project: 01
author: Nathan Wooddell
email: Nathanw2@umbc.edu
student id: TC56612
description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"

/*
In this project, you are going to implement a number of functions to
create a simple linux shell interface to perform basic linux commands
*/


//DEFINE THE FUNCTION PROTOTYPES
int user_prompt_loop();
char* get_user_command();
char** parse_command();
int execute_command();
int handle_proc();


/*
    Write the main function that checks the number of argument passed to ensure
    no command-line arguments are passed; if the number of argument is greater
    than 1 then exit the shell with a message to stderr and return value of 1
    otherwise call the user_prompt_loop() function to get user input repeatedly
    until the user enters the "exit" command.
    */
int main(int argc, char **argv){
	// Variable Declaration
	int return_value = 0;

	// Main code body
	fprintf(stdout, "****** Welcome to the 'C'-Shell ****** \n");

	if(argc < 2){											// Check if any arguments are passed
    	return_value = user_prompt_loop();					// gets the ultimate return value from the prompt loop

    }else{
    	fprintf(stderr, "Shell received an argument: '%s' \n****** Shell cannot be run ****** \n", argv[1]);
    	return_value = 1;									// returns with a value of 1 if the shell doesn't run
    }

    return return_value;
}

/*
user_prompt_loop():
Get the user input using a loop until the user exits, prompting the user for a command.
Gets command and sends it to a parser, then compares the first element to the two
different commands ("/proc", and "exit"). If it's none of the commands,
send it to the execute_command() function. If the user decides to exit, then exit 0 or exit
with the user given value.
*/

int user_prompt_loop(void){

    // initialize variables
	char* input = NULL;
	char** parsed;
	int run = 0;

	// Main Program / Function Loop
	while(run == 0){
		printf(">> ");										// The prompt to the user
		input = get_user_command();							// gets a command from the user
		parsed = parse_command(input);						// Calls for the parsing of the user input

		if(strcmp(parsed[0], "exit") == 0 ){
			if(parsed[1]!= NULL){							// Check for an argument to the exit command
				run = atoi(parsed[1]);						// Try to convert string value to integers
			}
			free(input);									// free the string passed by the user
			free(parsed);									// free dyanmic array of arguments
			exit(run);										// default return code should be 0

		}else if((strcmp(parsed[0], "/proc")== 0)||
				(strcmp(parsed[0], "proc")== 0)){
			handle_proc(parsed);							// A function to handle proc style commands

		}else{
			execute_command(parsed);						// execute a command as indicated by the user
		}
		free(input);										// Free the string passed by the user
		free(parsed);										// Free the dynamic array of arguments


	}
	return 1;												// need to evlauate documentation
}

/*
get_user_command():
Take input of arbitrary size from the user and return to the user_prompt_loop()
*/

char* get_user_command(void){ 								// Get a command from the user
	// Variable Declaration
	char* input = NULL;
	size_t input_size = 0;

	// Main Function Code Block
	ssize_t trim = getline(&input, &input_size, stdin);		// The size of the getline is later used in input trimming

	// A helper to trim the string returned by getline
	if(trim > 0){
		if(input[trim-1] == '\n'){							// makes sure the line ends with a newline char (\n)
			input[trim-1] = 0;								// replaces the newline with 0
			trim--;											// ultimately not necessary, but a good habit
		}
	}

	return input;											// returns the trimmed input.
}

/*
parse_command():
Take command grabbed from the user and parse appropriately.
Example:
    user input: "ls -la"
    parsed output: ["ls", "-la", NULL]
Example:
    user input: "echo     hello                     world  "
    parsed output: ["echo", "hello", "world", NULL]
*/

char **parse_command(char *input){						// * Parse the command provided by the user on the spaces

	int parse_size = 128;								// used to allocate an appropriate amount of space - this is extra large, shouldn't be a problem...
	int i = 0;											// array indexing variable
	char** arguments = malloc(parse_size * sizeof(char*));
	char* argument;										// individual storage for an argument/command
	char* delimiter = " ";								// I may eventually be better off defining this in the strtok_r function
	//char* context;										// according to documentation, this should save the context of the user input

	argument = strtok(input, delimiter);				// takes from input until delimiter is found, and saves to the argument char*
	while(argument != NULL){							// Loop controlled by array values
		arguments[i] = argument;						// places an argument in the array
		i++;											// array positioning control

		// NEED TO INCLUDE A SIZE INCREASE WHEN ARGUEMNTS GETS TOO BIG!

		argument = strtok(NULL, delimiter);				// Get the next argument
	}
	arguments[i] = NULL;								// end the array with a NULL value -> arg[hello, 1, 2, NULL]
	return arguments;
}

/*
execute_command():
Execute the parsed command if the commands are neither /proc nor exit;
fork a process and execute the parsed command inside the child process
*/

int execute_command(char **parsed){			// ASSESS RETURN VALUES

	// Variable Declaration
	pid_t process_id;
	//pid_t wait_id;
	int wait_status;
	int exec_value;

	// Fork a process to execute from
	process_id = fork();							// Forks the process which can be executed from

	// Process Handling - Differentiate between fork return codes
	if(process_id < 0){								// Indicates a major issue with fork
		fprintf(stdout, "MAJOR ERROR IN FORKING PROCESS");
		return 0;

	}else if(process_id == 0){						// This indicates we are within the child process.
		exec_value = execvp(parsed[0], parsed);		// execute the specified command

		if(exec_value == -1){
			fprintf(stdout, "Specified command doesn't exist, or failed to run \n");
			exit(1);								// This exits just the parent process

		}
	}else{											// Indicates that we are within the parent process.
		waitpid(process_id, &wait_status,0);		// Here we need to wait for the child to return
	}

	return 1;
}

/*
handle_proc();
Takes the parsed command, and assuming the first string is proc, prints
information from the specified /proc system. This funtion needs to be able
to handle the following;
  - cpuinfo
  - loadavg
  - pid/status
  - pid/environ
  - pid/sched
This function has been added to the template as a helper/way to isolate /proc code
 */
int handle_proc(char** parsed){
	FILE* fileptr;
	char full_path[255];
	char* printbuffer;
	size_t buffersize = 0;
	ssize_t readlen;

	if(parsed[1] == NULL){
		fprintf(stdout, "All proc commands must have an argument. \n");
		return -1;
	}

	//This is handling for the 'proc' vs '/proc' situation
	if(parsed[0][0] == '/'){						// handles '/proc'
		strcpy(full_path, parsed[0]);
		strncat(full_path, parsed[1], strlen(parsed[1]));
	}else{											// handles 'proc'
		strcpy(full_path, "/");
		strcat(full_path, parsed[0]);
		strcat(full_path, parsed[1]);
	}


	if((fileptr = fopen(full_path, "r"))){			// Makes sure that the file exists

		// reads from the file, overwriting the line as it reads.
		while((readlen = getline(&printbuffer, &buffersize, fileptr)) != -1){
			// Prints the line before it is overwritten.
			printf("%s", printbuffer);
		}

		free(printbuffer);							// frees the getline() buffer.
		fclose(fileptr);							// closes the file

	}else{											// Handles an invalid file
		fprintf(stdout, "Please select a valid proc file to read from. \n");
		return -2;
	}

	return 1;
}
