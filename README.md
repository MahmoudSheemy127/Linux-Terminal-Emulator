# Linux-Terminal-Emulator
C++ application that Emulates Linux shell commands in terminal. It was a college project for an Operating System course that was aimed to apply practical knowlegde on interprocess communication, memeory management and 
synchronization. 
The application simply analyzes and parse linux shell commands and execute them. the complexity of the program lies in multiple cases such as handling multiple user commands through pipelining, executing 
user commands at the background and forwarding execution output to output files and file appending.

The application is broken down into the following steps:
1. Take user input from terminal.
2. Analyze user command input and breaks it into tokens through lex analyzer.
3. Parse user input command according to set of rules or grammer defined by a yacc parser.
4. Run C++ handler methods corresponding to different commands and show output on terminal.


## Installation and Usage

> **Note:** Make sure you run the application on a UNIX environment since SystemV Unix library is used in the application.

1. Clone the github repo
2. run ```sudo apt-get install lex``` to install lex analyzer
3. run ```sudo apt-get install yacc``` to install yacc parser
4. run ```make clean``` to clear any object and extra files used during the compilation process.
5. run ```make all``` to build the whole project
6. run ```./shell``` to start using the shell and entering commands

demo preview:

![Alt Text](https://im3.ezgif.com/tmp/ezgif-3-93e0a30f8d.gif)

## File Structure
1. **shell.l**: lex file for tokenizing the input syntax and apply lexical analysis
2. **shell.y**: yacc file for grammer rules and parsing token stream
3. **command.h**: C++ header file to define command class and handler objects for command execution
4. **command.cc**: Implementation of the handler objects methods
5. **Makefile**: for building the project

