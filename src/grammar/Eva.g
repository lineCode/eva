/*Lexer and Parser grammars*/
grammar Eva;

options 
{
	language = C;
	output = AST;
	ASTLabelType = pANTLR3_BASE_TREE;
}

/*Tokens used for rewrite rules*/
tokens
{
	VAR;
	GROUP;
	BIN;
	DEC;
	HEX;
	OPTION;
}

program
		:	(statement NEWLINE)+			-> statement+
		;

statement
		:	bitwise_expression				->	bitwise_expression
		|	ID '=' bitwise_expression		->	^('=' ID bitwise_expression)
		|	NEWLINE
		;

bitwise_expression
		:	(a1=comparision_expression		->	$a1)
			(BIT_OP a2=comparision_expression->	^(BIT_OP $bitwise_expression $a2))*
		;

comparision_expression
		:	(a1=shift_expression			->	$a1)
			(COM_OP a2=shift_expression		->	^(COM_OP $comparision_expression $a2))*
		;

shift_expression
		:	(a1=add_expression				->	$a1)
			(SH_OP a2=add_expression		->	^(SH_OP $shift_expression $a2))*
		;

add_expression
		:	(m1=mult_expression				->	$m1)
			(ADD_OP m2=mult_expression		->	^(ADD_OP $add_expression $m2))*
		;

mult_expression
		:	(a1=uni_expression				->	$a1)
			(MUL_OP a2=uni_expression		->	^(MUL_OP $mult_expression $a2))*
		;

uni_expression
		:	a1=atom							->	$a1
		|	UNI_OP a2=atom					->	^(UNI_OP $a2)
		;

atom
		:	ID								->	^(VAR ID)
		|	INT_BIN							->	^(BIN INT_BIN)
		|	INT_DEC							->	^(DEC INT_DEC)
		|	INT_HEX							->	^(HEX INT_HEX)
		|	'('bitwise_expression')'		->	^(GROUP bitwise_expression)
		;

/*lexer grammar*/		
ID			:	('a'..'z' | 'A'..'Z' | '_')('a'..'z' | 'A'..'Z' | '0'..'9' | '_')*;
INT_BIN		:	'0b'('0' | '1')+;
INT_DEC		:	('0'..'9'*'.''0'..'9'+) | ('0'..'9'+);
INT_HEX		:	'0x'('0'..'9' | 'a'..'f' | 'A'..'F')+;

ADD_OP		:	'+' | '-';
MUL_OP		:	'*' | '/' | '%';
SH_OP		:	'>>'|'<<';
COM_OP		:	'<' | '<=' | '>' | '>=' | '==' | '!=';
BIT_OP		:	'&' | '^' | '|';
UNI_OP		:	'!' | '~' | '++' | '--' | '-' | '+';

WS			:	( ' ' | '\t')					{$channel=HIDDEN;};
NEWLINE		:	( '\n'| '\r')+;
