
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD 

%token 	NOTOKEN GREAT NEWLINE PIPE GOOD BACKGROUND GREAT2 ERROR_APPEND ERROR

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}

#define yylex yylex
#include <stdio.h>
#include "command.h"
#include <string.h>

%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	command_and_args iomodifier_opt iomodifier_opt pipe simple_command 
	|command_and_args iomodifier_opt iomodifier_opt background NEWLINE {
		if(Command::_currentCommand._exitflag==1){ 
		printf("   Yacc: Execute command\n");
		
		}
		
		Command::_currentCommand.execute();
	}
	| NEWLINE
	
	| error NEWLINE { yyerrok; }
	;
background:
	BACKGROUND{
		Command::_currentCommand._background=1;
	}
	|
	;
	
command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	} 
	|
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

command_word:
	WORD {
	if(strcmp($1,"exit")!=0){ 
	Command::_currentCommand._exitflag=1;
               printf("   Yacc: insert command \"%s\"\n", $1);
	}
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;
pipe:
	 PIPE 
	 |
	;
iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	| GREAT2 WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._appendFile = $2;
		
	}
	| GOOD WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	| ERROR WORD {
		printf("   Yacc: insert error \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
	}
	| ERROR_APPEND WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._errFlag = 1;
	}
	| /* can be empty */ 
	;

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
