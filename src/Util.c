#include <stdio.h>
#include "Expr.h"

#define NUM_HEX_PER_BYTE	2
#define NUM_DEC_PER_BYTE	3
#define NUM_BIN_PER_BYTE	8
#define END_OF_LINE			2

number_t ExprResult=-1;

char * ConvertNumberToString(number_t number)
{
	char *buffer, *result;
	int tot_chars, print_format, size;
	
	size =	(sizeof(number_t) * NUM_HEX_PER_BYTE) + END_OF_LINE + 2 +
			(sizeof(number_t) * NUM_DEC_PER_BYTE) + END_OF_LINE + 2 +
			(sizeof(number_t) * NUM_BIN_PER_BYTE) + END_OF_LINE;

	result = buffer = (char *)malloc(size);
	if (buffer == NULL) {
		return NULL;
	}

	/*Get the symbol value for print format - to decide printing in dec/hex/bin format*/
	print_format = GetIdentifierValue("print_format");
	if (print_format == 0) {
		print_format = 1;
	}
	if (print_format & 1) {
		tot_chars = sprintf(buffer, "%#ld ", number);
		buffer += tot_chars;
	}
	if (print_format & 2) {
		tot_chars = sprintf(buffer, "%#lx ", number);
		buffer += tot_chars;
	}
	if (print_format & 4) {
		int i;
		int bits_per_number = sizeof(number) * NUM_BIN_PER_BYTE;
		
		buffer[0] = '0';
		buffer[1] = 'b';
		buffer += 2;
		for(i = bits_per_number;i > 0; i--)	{
			buffer[bits_per_number-i] = '0' + ( (number>>(i-1)) & 1) ;
		}
		buffer[bits_per_number] = ' ';
		buffer[bits_per_number+1] = 0;
	}
	return result;
}

number_t ParseBinaryNumber(char *string)
{
	number_t result=0;
	int i,j;
	
	/*skip "0b"*/
	string+=2;
	for(j=0, i = strlen(string) - 1;i >= 0;i--, j++) {
		int number = string[i] - '0';
		result += (number<<j);
	}
	
	return result;
}

void StoreResult(number_t number)
{
	ExprResult = number;
}
