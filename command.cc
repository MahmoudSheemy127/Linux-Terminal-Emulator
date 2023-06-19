
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include<time.h>

#include "command.h"


#define SINGLE_COMMAND_EXECUTION 1
#define MULTIPLE_COMMAND_EXECUTION 0

int ExecutionType;



SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_appendFile = 0;
	_errFile = 0;
	_errFlag = 0;
	_background = 0;
	_exitflag =0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}
	if(_appendFile) {
		free(_appendFile);
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_appendFile = 0;
	_errFile = 0;
	_errFlag = 0;
	_background = 0;
	_exitflag =0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}
int bye(void){
	printf("\n\tGood  bye!!\n");
	exit(0);
}
void child_terminat(int signum){
	time_t t;
    	time(&t);
    	
	FILE *ptr;
	
	ptr=fopen("logs","w");
	fprintf(ptr,"%s",ctime(&t) );
	fclose(ptr);
}
void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	
	//default execution type
	ExecutionType = SINGLE_COMMAND_EXECUTION;

	// Print contents of Command data structure
	if(_exitflag == 1){
	print();
	}

	
	// Add execution here
	int i,id;
	char *command;
	
	//pipe file descriptors
	int fdp[_numberOfSimpleCommands-1][2];
	
	//default descriptors for  I/O
	int defaultInp = dup(0);
	int defaultOut = dup(1);
	int defaultErr = dup(2);

	//Create pipes according to number of commands
	for(i=0;i<_numberOfSimpleCommands-1;i++)
	{
		if(pipe(fdp[i]) == -1)
		{
			printf("Error with piping!");
		}
	}



	if(_numberOfSimpleCommands > 1)
	{
		//type of execution is multiple commands (piping)
		ExecutionType = MULTIPLE_COMMAND_EXECUTION;
	}



	for(i=0;i<_numberOfSimpleCommands;i++)
	{
		if(strcmp(_simpleCommands[i]->_arguments[ 0 ],"exit")==0)
		{ 
			bye();					
		}
		
		if(strcmp(_simpleCommands[i]->_arguments[ 0 ],"cd")==0){ 
			if(_simpleCommands[i]->_arguments[ 1 ]==NULL)
			{
				printf("missing directory\n ");
			}else if(chdir(_simpleCommands[i]->_arguments[ 1 ])!=0)
			{
				chdir(getenv("HOME"));
		}
		continue;		
		}

		//fork
		id = fork();
		if(id == 0)
		{

			if(ExecutionType == MULTIPLE_COMMAND_EXECUTION)
			{


				if(i == 0)
				{
					//First Command					
					dup2(fdp[i][1],1);
					dup2(defaultInp,0);
					dup2(defaultErr,2);
					close(defaultInp);
					close(defaultErr);
				}
				else if(i == _numberOfSimpleCommands-1)
				{
					//Last Command
					dup2(fdp[i-1][0],0);
					dup2(defaultOut,1);
					dup2(defaultErr,2);
					close(defaultOut);
					close(defaultErr);
				}
				else
				{
					//nth Command
					dup2(fdp[i-1][0],0);
					dup2(fdp[i][1],1);
					dup2(defaultErr,2);
					close(defaultErr);
				}
				int j;

				//close all descriptors
				for(j=0;j<_numberOfSimpleCommands-1;j++)
				{
					close(fdp[j][0]);
					close(fdp[j][1]);
				}

			}
			
			//check for output
			if(_outFile != 0 && (ExecutionType == SINGLE_COMMAND_EXECUTION || i == _numberOfSimpleCommands-1))
			{
				//create new file for out
				int fileW = creat(_outFile,0666);
				dup2(fileW,1);
				close(fileW);
			}

			//check for input
			if(_inputFile != 0 && (ExecutionType == SINGLE_COMMAND_EXECUTION || i == _numberOfSimpleCommands-1))
			{
				int fileR = open(_inputFile,O_RDONLY,0777);
				dup2(fileR,0);
				close(fileR);
			}


			//check for error
			if(_errFile != 0 && (ExecutionType == SINGLE_COMMAND_EXECUTION || i == _numberOfSimpleCommands-1))
			{
				int fileE;
				if(_errFlag == 1)
				{
					fileE = open(_errFile,O_RDWR | O_APPEND,0777);
				}
				else
				{
					fileE = creat(_errFile,0666);
				}
				dup2(fileE,2);
				close(fileE);
			}

			//check for append
			if(_appendFile != 0 && (ExecutionType == SINGLE_COMMAND_EXECUTION || i == _numberOfSimpleCommands-1))
			{
				int fileA = open(_appendFile,O_RDWR | O_APPEND,0777);
				dup2(fileA,1);
				close(fileA);
			}
			

			//child process
			command = _simpleCommands[i]->_arguments[0];
			if(execvp(command,_simpleCommands[i]->_arguments) == -1)
			{
				printf("Error status 2\n");
			}
		}
		else if(id == -1)
		{
			printf("Error status 1");
		}

	}
		
	 
	//restore redirection		
	dup2(defaultInp,0);
	dup2(defaultOut,1);
	dup2(defaultErr,2);
	
	//close all descriptors
	for(i=0;i<_numberOfSimpleCommands-1;i++)
	{
		close(fdp[i][0]);
		close(fdp[i][1]);
	}

	close(defaultInp);
	close(defaultOut);
	close(defaultErr);

	//wait for last process
	if(_background == 0)
	{
		waitpid(id,NULL,0);
	}


	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

int 
main()
{
	signal(SIGCHLD,child_terminat);
	signal(SIGINT,SIG_IGN);
	Command::_currentCommand.prompt();
	
	yyparse();
	return 0;
}
