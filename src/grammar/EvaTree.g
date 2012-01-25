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
		:	e = bitwise_expression
			{
				mpf_set(*fExpressionResult, *$e.result);
				FreeNumber($e.result);
			}
		|	^('=' ID e=bitwise_expression)
			{
				AssignValueToIdentifier((char *)$ID.text->chars, *$e.result);
				FreeNumber($e.result);
			}
		;

bitwise_expression returns [number_t *result]
		:	e = comparsion_expression
			{
				result = $e.result;
			}
		|	^(op=BIT_OP e1=bitwise_expression e2=comparsion_expression)
			{
				result = AllocateAndInitNumber();
				DoBitwiseOperation(*result, *$e1.result, *$e2.result, (char *)$op.text->chars);
				FreeNumber($e1.result);
				FreeNumber($e2.result);
			}
		;

comparsion_expression returns [number_t *result]
		:	e = shift_expression
			{
				result = $e.result;
			}
		|	^(op=COM_OP e1=comparsion_expression e2=shift_expression)
			{
				result = AllocateAndInitNumber();
				DoComparisionOperation(*result, *$e1.result, *$e2.result, (char *)$op.text->chars);
				FreeNumber($e1.result);
				FreeNumber($e2.result);
			}
		;

shift_expression returns [number_t *result]
		:	e = add_expression
			{
				result = $add_expression.result;
			}
		|	^(op=SH_OP e1=shift_expression e2=add_expression)
			{
				result = AllocateAndInitNumber();
				DoShiftOperation(*result, *$e1.result, *$e2.result, (char *)$op.text->chars);
				FreeNumber($e1.result);
				FreeNumber($e2.result);
			}
		;

add_expression returns [number_t *result]
		:	e = mult_expression
			{
				result = $mult_expression.result;
			}
		|	^(op=ADD_OP e1=add_expression e2=mult_expression)
			{
				result = AllocateAndInitNumber();
				DoArithematicOperation(*result, *$e1.result, *$e2.result, (char *)$op.text->chars);
				FreeNumber($e1.result);
				FreeNumber($e2.result);
			}
		;
		
mult_expression returns [number_t *result]
		:	e = uni_expression
			{
				result = $uni_expression.result;
			}
		|	^(op=MUL_OP e1=mult_expression e2=uni_expression)
			{
				result = AllocateAndInitNumber();
				DoArithematicOperation(*result, *$e1.result, *$e2.result, (char *)$op.text->chars);
				FreeNumber($e1.result);
				FreeNumber($e2.result);
			}
		;

uni_expression returns [number_t *result]
		:	e = atom
			{
				result = $atom.result;
			}
		|	^(op=UNI_OP e1=atom)
			{
				result = AllocateAndInitNumber();
				DoUniaryOperation(*result, *$e1.result, (char *)$op.text->chars);
				FreeNumber($e1.result);
			}
		;
		
atom returns [number_t *result]
		:	^(VAR ID)
			{
				result = AllocateAndInitNumber();
				AssignValueFromIdentifier(*result, (char *)$ID.text->chars);
			}
		|	^(BIN INT_BIN)
			{
				result = AllocateAndInitNumber();
				mpf_set_str(*result, (char *)&$INT_BIN.text->chars[2], 2);
			}
		|	^(DEC INT_DEC)
			{
				result = AllocateAndInitNumber();
				mpf_set_str(*result, (char *)$INT_DEC.text->chars, 10);
			}
		|	^(HEX INT_HEX)
			{
				result = AllocateAndInitNumber();
				mpf_set_str(*result, (char *)&$INT_HEX.text->chars[2], 16);
			}
		|	^(GROUP e=bitwise_expression)
			{
				result = $e.result;
			}
		;
