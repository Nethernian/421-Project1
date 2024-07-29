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

/*
DEFINE THE FUNCTION PROTOTYPES
user_prompt_loop()
get_user_command()
parse_command()
execute_command()
*/

int main(int argc, char **argv)
{
    /*
    Write the main function that checks the number of argument passed to ensure
    no command-line arguments are passed; if the number of argument is greater
    than 1 then exit the shell with a message to stderr and return value of 1
    otherwise call the user_prompt_loop() function to get user input repeatedl
    until the user enters the "exit" command.
    */

	if(argc != 0) // return 1 and print the following in stderr
	{
		fprintf(stderr, "Args were passed to the shell - shell will not be run");
		return 1;
    }
	else if(argc)
	{
		fprintf(stderr, "Args were passed to the shell - shell will not be run");
		return 1;
	}
	else // no args passed, runs the user loop
	{
        return user_prompt_loop();
    }
	return 0;
}

/*
user_prompt_loop():
Get the user input using a loop until the user exits, prompting the user for a command.
Gets command and sends it to a parser, then compares the first element to the two
different commands ("/proc", and "exit"). If it's none of the commands,
send it to the execute_command() function. If the user decides to exit, then exit 0 or exit
with the user given value.
*/

int user_prompt_loop(void)
{
    // initialize variables
	char* input;
	size_t inputlen;
	char* parsed;
	char* first_element;
	int run = 0;
    /*
    loop:
        1. prompt the user to type command by printing >>
        2. get the user input using get_user_command() function
        3. parse the user input using parse_command() function
        Example:
            user input: "ls -la"
            parsed output: ["ls", "-la", NULL]
        4. compare the first element of the parsed output to "/proc"and "exit"
        5. if the first element is "/proc" then you have the open the /proc file system
           to read from it
            i) concat the full command:
                Ex: user input >>/proc /process_id_no/status
                    concated output: /proc/process_id_no/status
            ii) read from the file line by line. you may user fopen() and getline() functions
            iii) display the following information according to the user input from /proc
                a) Get the cpu information if the input is /proc/cpuinfo
                - Cpu Mhz
                - Cache size
                - Cpu cores
                - Address sizes
                b) Get the number of currently running processes from /proc/loadavg
                c) Get how many seconds your box has been up, and how many seconds it has been idle from /proc/uptime
                d) Get the following information from /proc/process_id_no/status
                - the vm size of the virtual memory allocated the vbox
                - the most memory used vmpeak
                - the process state
                - the parent pid
                - the number of threads
                - number of voluntary context switches
                - number of involuntary context switches
                e) display the list of environment variables from /proc/process_id_no/environ
                f) display the performance information if the user input is /proc/process_id_no/sched
        6. if the first element is "exit" the use the exit() function to terminate the program
        7. otherwise pass the parsed command to execute_command() function
        8. free the allocated memory using the free() function
    */

    /*
    Functions you may need:
        get_user_command(), parse_command(), execute_command(), strcmp(), strcat(),
        strlen(), strncmp(), fopen(), fclose(), getline(), isdigit(), atoi(), fgetc(),
        or any other useful functions
    */

	while(run == 0){ 				// Run until failure, or message from user
		printf(">> ");					// a prompt for the user to enter input
		input = get_user_command();			// get a command from the user, and process it
		inputlen = strlen(input);			// get the length of the user command (includes spaces and NULL)

		parsed = parse_command(input, inputlen);	// parse the user command to separate portions of a string


		first_element = parsed[0];			// gather the first element of the command, so we can check it against built in elements.

	/*	if(first_element == "exit")		// handles the exit command
		{
			if(parsed[1] != NULL){			// Evaluates whether whether a return code value was passed to the exit command.
				run = parsed[1];
				free(parsed);
				return run;
			}
			else{
				free(parsed);
				return 0;
			}
		}
		else if(first_element == "/proc")	// handles the /proc command
		{
			// code for /proc call
		}
		else
		{
			execute_command(input);		// handles user commands
		} */
		free(input);
	}
}

/*
get_user_command():Take input of arbitrary size from the user and return to the user_prompt_loop()
*/

char *get_user_command(void) // done
{
    /*
    Functions you may need:
        malloc(), realloc(), getline(), fgetc(), or any other similar functions.
   */
	// Variable Definition
	char* input = "";				// will store the user input.
	size_t input_len = 0;				// stores the length of the user buffer.
	ssize_t linein;						// likely redundant, was originally intended to store length of getline.

	// Main function
	getline(&input, &input_len, stdin);		// getline should automatically determine the length of the string, and allocate memory.
	printf(input); // DELETE ME
	return input;					// The size of this will need to be calcualted when it is used.
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

char *parse_command(char *input)
{
    /*
    Functions you may need:
        malloc(), realloc(), free(), strlen(), first_unquoted_space(), unescape()
	NOTES ON UNESCAPE AND FIRST_UNQUOTED_SPACE:
		- UNESCAPE : Transform all escape sequences...?
		- FIRST_UNQ... : Finds the first unquoted/unescaped space in a char array
		- COUNT_SPACES : Finds the # of objects found by isspace()
    */
	/* my logic for this:
		1. determine location of spaces
		2. instantiate array of this length
		3. divide the string at these spaces
		4. place the strings into an array
		5. return a pointer to the array
	*/
	size_t input_spaces = count_spaces(input);			// will store the total number of spaces in the string
	size_t input_length = strlen(input);				// the original length of the string

	char *parsed[input_spaces+1];					// Will be able to hold the correct number of args passed and one more for NULL.

	char* token = strtok_r(input, " ");			// Get the first token from the cmd_string
	int array_pos = 0;

	while(token != NULL){						// Handles Subsequent tokens
		parsed[array_pos] = token;
		array_pos += 1;						// Moves to next part of array.
		token = strtok_r(input, " ");
	}

	return parsed;
}

/*
execute_command():
Execute the parsed command if the commands are neither /proc nor exit;
fork a process and execute the parsed command inside the child process
*/

/*execute_command()*/
{
    /*
    Functions you may need:
        fork(), execvp(), waitpid(), and any other useful function
    */

    /*
    ENTER YOUR CODE HERE
    */
}
