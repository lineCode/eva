#ifndef EVA_MAIN_H
#define EVA_MAIN_H

#ifdef _WIN32 
#include <mpir.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/*hack to avoid libantlr warning messages*/
#undef NULL
#define NULL (0)

typedef mpf_t number_t;
typedef mpz_t integer_t;

extern number_t *fExpressionResult;
void StoreResult(number_t number);

typedef enum {
	PRINT_FORMAT_BIN = 1,
	PRINT_FORMAT_DEC = 2,
	PRINT_FORMAT_HEX = 4,
}print_format_t;

int InitEva();

number_t *AllocateAndInitNumber();
void FreeNumber(number_t *result);

integer_t *AllocateAndInitInteger();
void FreeInteger(integer_t *i);

void AssignValueToIdentifier(char *Id, number_t value);
void AssignValueFromIdentifier(number_t result, char *Id);
unsigned int GetIdentifierValueAsNativeInteger(char *Id);

int EvaluateExpressions(const char *buffer, int buf_length, int is_filename);
number_t *ParseBinaryNumber(char *string);
void SetOptions(char *option, int value);
char * ConvertNumberToString(number_t number);
void DoArithematicOperation(number_t result, number_t number1, number_t number2, char *op);
void DoShiftOperation(number_t result, number_t number1, number_t number2, char *op);
void DoComparisionOperation(number_t result, number_t number1, number_t number2, char *op);
void DoBitwiseOperation(number_t result, number_t number1, number_t number2, char *op);
void DoUniaryOperation(number_t result, number_t number1, char *op);
char * ConvertNumberToString(number_t number);

#ifdef __cplusplus
    }
#endif
#endif
