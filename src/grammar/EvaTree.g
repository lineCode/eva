/*Tree walker grammar for Eva*/
tree grammar EvaTree;

options 
{
	language=C;
	ASTLabelType=pANTLR3_BASE_TREE;
	tokenVocab=Eva;
}
@includes
{
	#include <Expr.h>
}

program		
		:	statement+							
		;

statement	
		:	bitwise_expression						{PrintResult($bitwise_expression.result);}
		|	^('=' ID bitwise_expression)					{AssignValueToIdentifier((char *)$ID.text->chars, $bitwise_expression.result);}
		;

bitwise_expression returns [number_t result]
		:	comparsion_expression						{result=$comparsion_expression.result;}
		|	^(op=BIT_OP e1=bitwise_expression e2=comparsion_expression)	{result = DoBitwiseOperation($e1.result, $e2.result, (char *)$op.text->chars);}
		;

comparsion_expression returns [number_t result]
		:	shift_expression						{result=$shift_expression.result;}
		|	^(op=COM_OP e1=comparsion_expression e2=shift_expression)	{result = DoComparisionOperation($e1.result, $e2.result, (char *)$op.text->chars);}
		;

shift_expression returns [number_t result]
		:	add_expression							{result=$add_expression.result;}
		|	^(op=SH_OP e1=shift_expression e2=add_expression)		{result = DoShiftOperation($e1.result, $e2.result, (char *)$op.text->chars);}
		;

add_expression returns [number_t result]
		:	mult_expression							{result=$mult_expression.result;}
		|	^(op=ADD_OP e1=add_expression e2=mult_expression )		{result = DoArithematicOperation($e1.result, $e2.result, (char *)$op.text->chars);}
		;
		
mult_expression returns [number_t result]
		:	uni_expression							{result=$uni_expression.result;}
		|	^(op=MUL_OP e1=mult_expression e2=uni_expression)		{result = DoArithematicOperation($e1.result, $e2.result, (char *)$op.text->chars);}
		;

uni_expression returns [number_t result]
		:	atom								{result=$atom.result;}
		|	^(op=UNI_OP e1=atom)						{result = DoUniaryOperation($e1.result, (char *)$op.text->chars);}
		;
		
atom returns [number_t result]
		:	^(VAR ID)							{result = GetIdentifierValue( (char *)$ID.text->chars);}
		|	^(BIN INT_BIN)							{result = ParseBinaryNumber( (char *)$INT_BIN.text->chars);}
		|	^(DEC INT_DEC)							{result = strtoul( (char *)$INT_DEC.text->chars,NULL,0);}
		|	^(HEX INT_HEX)							{result = strtoul( (char *)$INT_HEX.text->chars,NULL,0);}
		|	^(GROUP bitwise_expression)					{result = $bitwise_expression.result;}
		;
		