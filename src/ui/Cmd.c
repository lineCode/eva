#include <stdio.h>
#include "Expr.h"

int ANTLR3_CDECL main(int argc, char *argv[])
{
	char file[]="./input";
	char expression[]="1+1+(3*3)";
	
	InitEva();
	
	EvaluateExpressions(file, strlen(file), 1);
	EvaluateExpressions(expression, strlen(expression), 0);
}

void PrintResult(number_t number)
{
	if( print_format & 1)
			printf("%ld\n\r", number);
	if( print_format & 2)
			printf("%#lx\n\r", number);
	if( print_format & 4)
	{
		int i;
		printf("0b");
		for(i=(sizeof(number)*8)-1;i>=0;i--)
			printf("%1d", (number>>i) & 1 );

		printf("\n\r");
	}
}
