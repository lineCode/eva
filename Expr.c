/*!  Container file for all the operations invoked from the Tree Walker
 */
#include <EvaLexer.h>
#include <EvaParser.h>
#include <EvaTree.h>

/*! symbol table to hold variable name and value*/
pANTLR3_HASH_TABLE symtab;
typedef struct
{
	number_t value;
}symbol_value_t;

int print_format=1;

/*! Initialize the Expression Evalutor*/
int InitEva()
{
	/*Create symbol table for handling variables*/
	symtab = antlr3HashTableNew(1);
	if (symtab == NULL)
		return 1;
	return 0;
}

/*! Read expressions from file or memory and Evaluate 
*/
int EvaluateExpressions(const char *buffer, int buf_length, int is_filename)
{
	pANTLR3_INPUT_STREAM input_stream;
	pEvaLexer lxr;
	pANTLR3_COMMON_TOKEN_STREAM token_stream;
	pEvaParser psr;
	EvaParser_program_return eva_ast;
	pEvaTree treeParser;
	pANTLR3_COMMON_TREE_NODE_STREAM	nodes;

	/*Is it a file or memory*/
	if (is_filename)
		input_stream = antlr3AsciiFileStreamNew(pANTLR3_UINT8(buffer));
	else
		input_stream = antlr3NewAsciiStringCopyStream ( (pANTLR3_UINT8)buffer, (ANTLR3_UINT32) buf_length, NULL);
	if (input_stream==NULL)
		return 1;
	/*Invoke lexer and tokenzie*/
	lxr	= EvaLexerNew(input_stream);
	if (lxr==NULL)
		return 2;
	token_stream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));
	if ( token_stream == NULL )
		return 3;
	/*Parse the expression*/
	psr	= EvaParserNew(token_stream);
	if ( psr == NULL )
		return 4;
	if (psr->pParser->rec->state->errorCount > 0)
		return 5;
	
	/*create ast from the parser*/
	eva_ast = psr->program(psr);
	nodes = antlr3CommonTreeNodeStreamNewTree(eva_ast.tree, ANTLR3_SIZE_HINT);
	if ( nodes == NULL )
		return 7;
		
	/*Walk the tree and evaluate the expression*/
	treeParser = EvaTreeNew(nodes);
	if ( treeParser == NULL )
		return 8;
	treeParser->program(treeParser);

	/*cleanup*/
    psr->free(psr);
    token_stream->free(token_stream);
    lxr->free(lxr);
    input_stream->close(input_stream);

	return 0;
}

void AssignValueToIdentifier(char *Id, number_t value)
{
	symbol_value_t *s;
	s = (symbol_value_t *)symtab->get(symtab, (void *)Id);
	if( s )
		s->value = value;
	else
	{
		s = (symbol_value_t *)malloc(sizeof(symbol_value_t));
		s->value = value;
		symtab->put(symtab, Id, s, NULL);
	}
}

number_t GetIdentifierValue(char *Id)
{
	symbol_value_t *s;
	s = (symbol_value_t *)symtab->get(symtab, (void *)Id);
	if( s )
		return s->value;
	else
		return 0;
}

number_t ParseBinaryNumber(char *string)
{
	number_t result=0;
	int i,j;
	/*skip "0b"*/
	string+=2;
	for(j=0, i=strlen(string)-1;i>=0;i--,j++)
	{
		int number = string[i] - '0';
		result += (number<<j);
	}
	return result;
}

void SetOptions(char *option, int value)
{
	if( strcmp(option, "PRINT")==0 )
		print_format = value;
	else
		printf("Unknow set option %s\n",  option);
}

number_t DoArithematicOperation(number_t number1, number_t number2, char *op)
{
	switch( op[0] )
	{
		case '+':
			return number1 + number2;
		case '-':
			return number1 - number2;
		case '*':
			return number1 * number2;
		case '/':
			return number1 / number2;
		case '%':
			return number1 % number2;
	}
}

number_t DoShiftOperation(number_t number1, number_t number2, char *op)
{
	switch( op[0] )
	{
		case '<':
			return number1<<number2;
		case '>':
			return number1>>number2;
	}
}

number_t DoComparisionOperation(number_t number1, number_t number2, char *op)
{
	switch( op[0] )
	{
		case '<':
			if ( op[1] == '=' )
				return number1<=number2;
			else
				return number1<number2;
		case '>':
			if ( op[1] == '=' )
				return number1>=number2;
			else
				return number1>number2;
		case '=':
			if ( op[1] == '=' )
				return number1==number2;
			else
				return number1!=number2;
	}
}

number_t DoBitwiseOperation(number_t number1, number_t number2, char *op)
{
	switch( op[0] )
	{
		case '&':
			return number1 & number2;
		case '|':
			return number1 | number2;
		case '^':
			return number1 ^ number2;
	}
	
}

number_t DoUniaryOperation(number_t number1, char *op)
{
	switch( op[0] )
	{
		case '!':
			return !number1;
		case '~':
			return ~number1;
		case '+':
			if ( op[1]=='+' )
				return number1+1;
			else
				return number1;
		case '-':
			if ( op[1]=='-' )
				return number1-1;
			else
				return -number1;	
	}
}

/*! Test case for reading input from a file and also from a memory*/
#ifdef TEST
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
			printf("%ld\n", number);
	if( print_format & 2)
			printf("%#lx\n", number);
	if( print_format & 4)
	{
		int i;
		printf("0b");
		for(i=(sizeof(number)*8)-1;i>=0;i--)
			printf("%1d", (number>>i) & 1 );

		printf("\n");
	}
}
#endif
