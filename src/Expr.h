#ifndef EVA_MAIN_H
#define EVA_MAIN_H

typedef unsigned long number_t;
#ifdef __cplusplus
    extern "C" {
#endif

/*hack to avoid libantlr warning messages*/
#undef NULL
#define NULL (0)

extern number_t ExprResult;
void StoreResult(number_t number);

int InitEva();

void AssignValueToIdentifier(char *Id, number_t value);
int EvaluateExpressions(const char *buffer, int buf_length, int is_filename);
number_t GetIdentifierValue(char *Id);
number_t ParseBinaryNumber(char *string);
void SetOptions(char *option, int value);
char * ConvertNumberToString(number_t number);
number_t DoArithematicOperation(number_t number1, number_t number2, char *op);
number_t DoShiftOperation(number_t number1, number_t number2, char *op);
number_t DoComparisionOperation(number_t number1, number_t number2, char *op);
number_t DoBitwiseOperation(number_t number1, number_t number2, char *op);
number_t DoUniaryOperation(number_t number1, char *op);
char * ConvertNumberToString(number_t number);

#ifdef __cplusplus
    }
#endif
#endif
