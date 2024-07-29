 # Description:
I created a simple linux shell using C, I like to refer to it as a "C-Shell". The entirety of my code is contained in the file "shellfile.c". This repository contains a makefile which will build the commandline tool.

# shellfile.c Functions
This is a list of the functions as implemented for this project.

## int user_prompt_loop();
This function is meant to prompt the user for input using the ">>" symbol. 
The user will then enter their input which when processed will be used to 
run commands or access the shell built-in functions.
The central part of this function is the main loop which runs until the user
enters the exit command. This should free any allocated data and exit the
the program.
There are two built in functions in this version of the shell; /proc and exit.
exit is described above, while proc is a little bit more complex and as of yet
not implemented.

## char* get_user_command();
This function is designed to get the user command. The current implementation 
of this command uses the getline() function to get input from the user.
This input currently is causing a small issue with return characters, and as such I
will need to revaluate before I can begin further testing.

## char** parse_command();
This function parses the command gathered through the get_user_command() function
which is described above. This function takes an object of type char*, and parses it
on every present space character. The parse command places all of these objects into
an array of char pointers, where they can be returned, and used to run commands.

## int execute_command();
The execute_command() function is responsible for running the function parsed in the
parse command function. This function starts by evaluating the first element of the parsed
array of strings, and then executing the command provided therein, using subsequent parts
of the array as arguments to the original command.

## int handle_proc();
Handles the shell builtin proc function. This function takes the parsed string, and ensures
that the proc command can be run with the information passed to it. The Proc function then
creates a file path and attempts to access it. In the event that the proc system is unable
to access the filepath specified by the user, and error is sent. This proc funtion is written
in a way that it would be easy to adapt to other file reading commands / my own version of the
cat command

# Disclaimer
This was completed for UMBC's CMSC 421 Course, and I am open to removing it from github upon request from UMBC faculty. 
If you would like to request this repository's removal, please send me an email at **nathanwooddell@gmail.com**