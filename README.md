**Author**
===========
Name: Sourav Nath
Roll: 2025201054
Programme: M.Tech (CSE)

**Project Title**
=================
POSIX Shell Implementation

**Project Description**
=======================
I implemented a shell which support semicolon separated list of commands , also support '&' operator which lets a program run in the background after printing the process id of the newly created process

**Files Overview**
=======================
***---Under src Directory---***
## Main.cpp - Entry point of Shell. 
>> This file control all the logics which decide when a user write any command on terminal then how that command should parse and what function should be called for that       commands etc.

## shell.cpp – Main shell implementation, handles command parsing, execution, and pipelines.
>>Defination of all functions are implemented here. this file contain defination of all the commands like ls,cd,pinfo,search,pwd,echo etc.

***---Under include Directory---***
## shell.h – Header file with function declarations and macros.
>>This file contain all the header files. which indicates what header files and what libraries we have used in out code.

***---Other Files (Root Directory)----***
## Makefile – Build instructions for compiling the shell.
>> This is like guide which helps to run our programme in easy way.
## shellHistory.txt – Stores command history (created at runtime).
>>To implement history command in shell we have to store the previous commands so we will be creating shellHistory.txt which will store all the history.
## README.md – Project description and documentation.
>>here we have write a brief of our project like what is our project about, what we have implemented and how to run this programme etc.

**How to Run**
===================
1)Open a terminal in the project directory.

2)Build the shell using make:
                make
3)Run the shell executable:
                ./shell

4)Use the shell like a normal terminal. Built-in commands (cd, pwd, echo, ls, pinfo, search) and system commands can be run.

5)To exit the shell, use exit or press Ctrl-D.


**Dependencies**
=============================
Libraries:
-------------
<iostream> – For input/output operations.

<string> – For handling strings.

<vector> – To store command tokens and arguments.

<unistd.h> – For system calls like fork(), exec(), chdir(), getcwd().

<sys/types.h> – For data types like pid_t.

<sys/wait.h> – For wait() and waitpid() to handle child processes.

<fcntl.h> – For file operations, especially for redirection.

<signal.h> – To handle signals like SIGINT.

<readline/readline.h> & <readline/history.h> – For command-line input with history support.

<limits.h> – For constants like PATH_MAX.

Tools:
--------
g++ – Compiler to build the project.

make – To automate compilation using the Makefile.

Linux/Unix environment – Needed for system calls and shell behavior.


**Features Implemented**
=========================

1.Display requirement: When we execute this code, a shell prompt of the following form appear along
with it:
_username_@_system_name_:_current_directory_>


2.This shell includes built-in commands like cd (which supports ., .., -, and ~ and shows the full path after changing directories), pwd (to display the current directory), and echo (to print text with spaces and tabs as-is). It also supports the ls command with -a and -l flags, allowing you to list files and directories in any order, with ls -l showing detailed file information. Besides these, the shell can run regular Linux commands, handle background processes using &, support piping and input/output redirection (<, >, >>), keep a history of commands, and respond to signals like Ctrl+C for stopping foreground tasks.

3.Implement the ls command with its two flags “-a” and “-l” with all the following cases.

4.The shell runs system commands in the foreground (waits for completion) or background (continues running, shows PID). Multiple background processes are supported, and non-built-in commands like gedit or vi can run in either mode.

5.The pinfo command displays process information for the shell, including process status, memory usage, and executable path. popen() is not used for its implementation.

6.The search command looks for a file or folder in the current directory and all its subdirectories. It prints True if found and False if not.

7.The shell supports input (<) and output (> or >>) redirection. Commands can take input from a file and/or send output to a file. > overwrites the file, >> appends, and errors are shown if the input file doesn’t exist.

8.The shell supports pipes (|) to send the output of one command as input to another. Any number of commands can be piped.

9.Redirection with pipeline Input/output redirection can occur within command pipelines, as the examples
below show. Your shell should be able to handle this.

10.The shell handles basic signals:

    Ctrl-Z – Stops the current foreground process and moves it to the background.

    Ctrl-C – Interrupts the current foreground process.

    Ctrl-D – Exits the shell without affecting the terminal.
    
11.The shell supports TAB autocomplete for commands and files/directories. Single matches are completed automatically; multiple matches show all options or fill the common prefix.

12.This shell support history. which enable us to see last 20 history command and also help us to navigate through the previous commands.
    
