#ifndef EVA_MAIN_H
#define EVA_MAIN_H

typedef unsigned long long number_t;
int InitEva();
void PrintResult(number_t number);
void AssignValueToIdentifier(char *Id, number_t value);
int EvaluateExpressions(const char *buffer, int buf_length, int is_filename);
number_t GetIdentifierValue(char *Id);
number_t ParseBinaryNumber(char *string);
void SetOptions(char *option, int value);
number_t DoArithematicOperation(number_t number1, number_t number2, char *op);
number_t DoShiftOperation(number_t number1, number_t number2, char *op);
number_t DoComparisionOperation(number_t number1, number_t number2, char *op);
number_t DoBitwiseOperation(number_t number1, number_t number2, char *op);
number_t DoUniaryOperation(number_t number1, char *op);

#endif
