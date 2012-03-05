#define MAX_HISTORY	256
char *experssionHistory[MAX_HISTORY]={0};
int expressionHistoryTail = 0;
int expressionHistoryPosition = 0;

static void RemoveNewLineCharacters(char *expression_string)
{
	int len;

	/* Remove newline characters */
	len = strlen(expression_string);
	if (len <= 0) {
		return;
	}
	len --;
	
	while (expression_string[len] == '\r' || expression_string[len] == '\n') {
		expression_string[len--] = 0;
	}

}

void AddToExperssionHistory(char *expression_string)
{
	RemoveNewLineCharacters(expression_string);

	/* Free the existing buffer */
	free(experssionHistory[expressionHistoryTail]);
	/* Assign the current buffer at tail */
	experssionHistory[expressionHistoryTail] = expression_string;
	
	/* Tail position management*/
	expressionHistoryTail++;
	if (expressionHistoryTail >= MAX_HISTORY) {
		expressionHistoryTail = 0;
	}

	/* set current history postion to tail */
	expressionHistoryPosition = expressionHistoryTail;
}

char *GetExpressionFromPrevLocation()
{
	if (expressionHistoryPosition > 0) {
		expressionHistoryPosition--;
	}
	
	return experssionHistory[expressionHistoryPosition];
}

char *GetExpressionFromNextLocation()
{
	if (expressionHistoryPosition < expressionHistoryTail) {
		expressionHistoryPosition++;
	}
	
	return experssionHistory[expressionHistoryPosition];
}