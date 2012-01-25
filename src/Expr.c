/*!  Container file for all the operations invoked from the Tree Walker
 */
#include <assert.h>
#include <EvaLexer.h>
#include <EvaParser.h>
#include <EvaTree.h>

/*! symbol table to hold variable name and value*/
pANTLR3_HASH_TABLE symtab;
typedef struct
{
	number_t value;
}symbol_value_t;

number_t *fExpressionResult;

/*! Initialize the Expression Evalutor*/
int InitEva()
{
	/*Create symbol table for handling variables*/
	symtab = antlr3HashTableNew(1);
	if (symtab == NULL) {
		return 1;
	}
	
	fExpressionResult = AllocateAndInitNumber();
	
	return 0;
}

/*! Allocate and Initialize a floating pointer number

	The caller is responsible for freeing the allocated number
*/
number_t *AllocateAndInitNumber()
{
	number_t *result;
	
	result = malloc(sizeof(number_t));
	if(result == NULL) {
		assert("malloc failure is not handled");
		return NULL;
	}
	
	mpf_init(*result);
	
	return result;
}

/*! Free the given floating pointer number */
void FreeNumber(number_t *result)
{
	mpf_clear(*result);
	free(result);
}

/*! Allocate and initialize a integer number
	
	The caller is responsible for freeing the allocated number
*/
integer_t *AllocateAndInitInteger()
{
	integer_t *result;
	
	result = malloc(sizeof(integer_t));
	if(result == NULL) {
		assert("malloc failure is not handled");
		return NULL;
	}
	
	mpz_init(*result);
	
	return result;
}

/*! Covert floating point number to integer

    The caller is responsible for freeing the allocated integer number
*/
integer_t *ConvertFloatToInteger(number_t num)
{
	integer_t *result;
	
	result = AllocateAndInitInteger();
	mpz_set_f(*result, num);
	
	return result;
}

/*! Free the given integer number */
void FreeInteger(integer_t *i)
{
	mpz_clear(*i);
	free(i);
}

/*! Convert the given integer operation(op) on the given floating 
    point number(number1) and stores the result in the given floating
	point number(result)
*/
#define DO_INTEGER_OPERATION1_ON_FLOAT(op, result, number1)			\
	{																\
		integer_t *integer1, *integer_result;						\
																	\
		integer1 = ConvertFloatToInteger(number1);					\
		integer_result = AllocateAndInitInteger();					\
																	\
		op(*integer_result, *integer1);								\
		mpf_set_z(result, *integer_result);							\
																	\
		FreeInteger(integer1);										\
		FreeInteger(integer_result);								\
	}

/*! Almost same as above macro but takes one float(number2) and 
    passes it to OP as integer
*/
#define DO_INTEGER_OPERATION2_ON_FLOAT(op, result, number1, number2)\
	{																\
		integer_t *integer1, *integer2, *integer_result;			\
																	\
		integer1 = ConvertFloatToInteger(number1);					\
		integer2 = ConvertFloatToInteger(number2);					\
		integer_result = AllocateAndInitInteger();					\
																	\
		op(*integer_result, *integer1, *integer2);					\
		mpf_set_z(result, *integer_result);							\
																	\
		FreeInteger(integer1);										\
		FreeInteger(integer2);										\
		FreeInteger(integer_result);								\
	}

/*! Almost same as above macro but takes one float(number2) and converts it to
	unsigned long before passing it to OP
*/
#define DO_INTEGER_OPERATION3_ON_FLOAT(op, result, number1, number2)\
	{																\
		integer_t *integer1, *integer_result;						\
		unsigned long native_integer;								\
																	\
		integer1 = ConvertFloatToInteger(number1);					\
		native_integer = mpf_get_ui(number2);						\
		integer_result = AllocateAndInitInteger();					\
																	\
		op(*integer_result, *integer1, native_integer);				\
		mpf_set_z(result, *integer_result);							\
																	\
		FreeInteger(integer1);										\
		FreeInteger(integer_result);								\
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
	if (is_filename) {
		input_stream = antlr3AsciiFileStreamNew( (pANTLR3_UINT8)buffer);
	} else {
		input_stream = antlr3NewAsciiStringCopyStream ( (pANTLR3_UINT8)buffer, (ANTLR3_UINT32) buf_length, NULL);
	}
	
	if (input_stream == NULL) {
		return -1;
	}
	
	/*Invoke lexer and tokenzie*/
	lxr	= EvaLexerNew(input_stream);
	if (lxr == NULL) {
		return -1;
	}
		
	token_stream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));
	if (token_stream == NULL) {
		return -1;
	}
	
	/*Parse the expression*/
	psr	= EvaParserNew(token_stream);
	if (psr == NULL) {
		return -1;
	}
	if (psr->pParser->rec->state->errorCount > 0) {
		return -1;
	}
	
	/*create ast from the parser*/
	eva_ast = psr->program(psr);
	nodes = antlr3CommonTreeNodeStreamNewTree(eva_ast.tree, ANTLR3_SIZE_HINT);
	if (nodes == NULL) {
		return -1;
	}
		
	/*Walk the tree and evaluate the expression*/
	treeParser = EvaTreeNew(nodes);
	if (treeParser == NULL)	{
		return -1;
	}
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
	if (s) {
		mpf_set(s->value, value);
	} else {
		s = (symbol_value_t *)malloc(sizeof(symbol_value_t));
		mpf_set(s->value, value);
		symtab->put(symtab, Id, s, NULL);
	}
}

void AssignValueFromIdentifier(number_t result, char *Id)
{
	symbol_value_t *s;
	
	s = (symbol_value_t *)symtab->get(symtab, (void *)Id);
	if (s) {
		mpf_set(result, s->value);
	}
}

unsigned int GetIdentifierValueAsNativeInteger(char *Id)
{
	symbol_value_t *s;
	
	s = (symbol_value_t *)symtab->get(symtab, (void *)Id);
	if (s) {
		return mpf_get_ui(s->value);
	}
	
	return -1;
}

void DoArithematicOperation(number_t result, number_t number1, number_t number2, char *op)
{
	switch (op[0])
	{
		case '+':
			mpf_add(result, number1, number2);
			break;
		case '-':
			mpf_sub(result, number1, number2);
			break;
		case '*':
			mpf_mul(result, number1, number2);
			break;
		case '/':
			mpf_div(result, number1, number2);
			break;
		case '%':
			DO_INTEGER_OPERATION2_ON_FLOAT(mpz_mod, result, number1, number2);
			break;
	}
}

void DoComparisionOperation(number_t result, number_t number1, number_t number2, char *op)
{
	int cmp_result;
	
	cmp_result = mpf_cmp(number1, number2);
	
	switch (op[0])
	{
		case '<':
			if (op[1] == '=') {
				mpf_set_si(result, cmp_result <= 0);
			} else {
				mpf_set_si(result, cmp_result < 0);
			}
			break;
		case '>':
			if(op[1] == '=') {
				mpf_set_si(result, cmp_result >= 0);
			} else {
				mpf_set_si(result, cmp_result > 0);
			}
			break;
		case '=':
			if (op[1] == '=') {
				mpf_set_si(result, cmp_result == 0);
			}
			else {
				mpf_set_si(result, cmp_result != 0);
			}
			break;
	}
}

void DoShiftOperation(number_t result, number_t number1, number_t number2, char *op)
{
	switch (op[0])
	{
		case '<':
			DO_INTEGER_OPERATION3_ON_FLOAT(mpz_mul_2exp, result, number1, number2);
			break;
		case '>':
			DO_INTEGER_OPERATION3_ON_FLOAT(mpz_tdiv_q_2exp, result, number1, number2);
			break;
	}
}

void DoBitwiseOperation(number_t result, number_t number1, number_t number2, char *op)
{
	switch (op[0])
	{
		case '&':
			DO_INTEGER_OPERATION2_ON_FLOAT(mpz_and, result, number1, number2);
			break;
		case '|':
			DO_INTEGER_OPERATION2_ON_FLOAT(mpz_ior, result, number1, number2);
			break;
		case '^':
			DO_INTEGER_OPERATION2_ON_FLOAT(mpz_xor, result, number1, number2);
			break;
	}
}

void DoUniaryOperation(number_t result, number_t number1, char *op)
{
	switch( op[0] )
	{
		case '!':
			mpf_set_si(result, mpf_cmp_ui(number1, 0));
			break;
		case '~':
			DO_INTEGER_OPERATION1_ON_FLOAT(mpz_com, result, number1);
			break;
		case '+':
			if (op[1] == '+') {
				mpf_add_ui(result, number1, 1);
			} else {
				mpf_set(result, number1);
			}
			break;
		case '-':
			if (op[1] == '-') {
				mpf_sub_ui(result, number1, 1);
			} else {
				mpf_sub(result, result, number1);
			}
			break;
	}
}
