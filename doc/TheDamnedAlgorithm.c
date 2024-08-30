#include <stdio.h>
#include <ctype.h>

#include "../lib/mstack.h"

char source[] = "(+(1-!3*(--4/5)))";
int  ndx=0;
char token ;
int err = 0 ;

char getToken(void)
{
    if ( source[ndx] != 0)
    {
    	return source[ndx++] ;
    }
	return 0 ;
}


// prototype

int startexpr(void) ;
int expr(void) ;


int term(void)
{
//
	stackType( char  , sPrefixOp ) ;
	stackNew ( sPrefixOp , 128 ) ;

	while ( token == '+'||	token == '-'||	token == '!')
	{
		stackPush ( sPrefixOp , token ) ;
		token=getToken();
	}

//
	switch ( token )
	{
		case '(' :

					if (token=='(')  
					{
					token=getToken();
					} 
					else 
					{ 
					printf ( "\n0 expr expected ( instead  [%c]",token) ;
					err=1;
					return 0 ;
					} ;

				expr();

					if (token==')') 
					{
					token=getToken();
					} 
					else 
					{ 
					printf ( "\n1 expr expected ( instead  [%c]",token) ;
					err=1;
					return 0 ;
					} ;
					err=0;
					while(stackSize(sPrefixOp))
					{
						printf ( "\n()prefix %c",stackTop(sPrefixOp) );
						stackPop(sPrefixOp);
					}

				return 1 ;
		break ;

		case '0' ... '9':

				printf ( "\nterm [%c]",token ) ;
				token=getToken() ;
				err=0;
				while(stackSize(sPrefixOp))
				{
					printf ( "\n[]prefix %c",stackTop(sPrefixOp) );
					stackPop(sPrefixOp);
				}
				return 1 ;
		break ;	

		default :

			printf ( "\n? term : sintax error {%c}",token ) ;
			err=1;

		break ;
	}

	return 0 ;
}

int moddivmul(void)
{
	term();

	while ( 	token=='*' 
			||  token=='/' 
			||  token=='%' 
	)
	{
		printf ( "\nmoddivmul [%c]",token ) ;
		token=getToken() ;
		term();
	}

	return 1 ;
}

int addsub(void)
{
 	moddivmul();

	while ( 	token=='+' 
			||  token=='-' 
	)
	{
		printf ( "\naddsub [%c]",token ) ;
		token=getToken() ;
		moddivmul();
	}

	return 1 ;
}

int expr(void)
{
	addsub();
}

int startexpr(void)
{
	if (token=='(')  
		token=getToken();
	else 
	{ 
			printf ( "\nstartexpr expected ( instead [%c]",token) ;
			err=1;
		return 0 ;
	} ;

    expr();


	if (token==')') 
	{
		token=getToken();
	} 
	else 
	{ 
			printf ( "\nstartexpr expected ( instead [%c]",token) ;
			err=1;
		return 0 ;
	} ;
}

int main (void )
{
	token=getToken();

	printf ( "\n-----------------");

	do{

		startexpr();
		printf ( "\n-----------------");

	} while(token!=0 && !err);

	printf ( "\nexit [%c]", token) ;
	printf("\n");

	return 0 ;
}
