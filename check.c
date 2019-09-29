#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define ARITHMETIC_OPERATOR 0
#define LOGICAL_OPERATOR 1
#define LOGICAL_NOT 2
#define ARITHMETIC_OPERAND 3
#define LOGICAL_OPERAND 4
#define TERMINAL_LOGICAL_OPERAND 6
#define TERMINAL_ARITHMETIC_OPERAND 7
#define EXPRESSION_START -1
#define EXPRESSION_FINISHED 10
#define UNKNOWN_TOKEN -10
#define UNKNOWN_EXPRESSION_TYPE 0
#define ARITHMETIC_EXPRESSION 1
#define LOGICAL_EXPRESSION 2

typedef struct _Node {
	struct _Node* previous;
	struct _Node* next;
	char* value;
} Node;

typedef struct _NodeList {
	struct _NodeList* previous;
	struct _NodeList* next;
	Node* value;
} NodeList;

typedef struct _StringArray {
	char** array;
	int length;
} StringArray;


int stringLength(char* string) {
	int length = 0;
	int i;
	for (i = 0; string[i] != '\0'; i++) {
		length++;
	}
	return length;
}

char* stringCopy(char* string, int size) {
	char* output = malloc(sizeof(char) * (size + 1));
	if (output == NULL) {
		printf("\nfailed malloc\n");
		return NULL;
	}
	int i;
	for (i = 0; i < size; i++) {
		output[i] = string[i];
	}
	output[size] = '\0';
	return output;
}

int stringEquals(char* string1, char* string2) {
	int i;
	for (i = 0; (string1[i] != '\0') || (string2[i] != '\0'); i++) {
		if (string1[i] != string2[i]) {
			return 0;
		}
	}
	return 1;
}

int stringArrayContains(StringArray check, char* token) {
	int i;
	for (i = 0; i < check.length; i++) {
		if (stringEquals(check.array[i], token)) {
			return 1;
		}
	}
	return 0;
}

Node* createNode(Node* prev, char* val, Node* nex) {
	Node* create = malloc(sizeof(Node));
	if (create == NULL) {
		printf("\nfailed malloc\n");
		return NULL;
	}
	create->previous = prev;
	create->value = val;
	create->next = nex;
	return create;
}

NodeList* createNodeList(NodeList* prev, Node* val, NodeList* nex) {
	NodeList* create = malloc(sizeof(NodeList));
	if (create == NULL) {
		printf("\nfailed malloc\n");
		return NULL;
	}
	create->previous = prev;
	create->value = val;
	create->next = nex;
	return create;
}

Node* insert(Node* head, char* val) {
	if (head != NULL) {
		Node* ptr = head;
		while (ptr->next != NULL) {
			ptr = ptr->next;
		}
		ptr->next = createNode(ptr, val, NULL);
	}
	else {
		head = createNode(NULL, val, NULL);
	}
	return head;
}

int freeNode(Node* head) {
	if (head != NULL) {
		Node* traverse = head;
		while (traverse->next != NULL) {
			Node* next = traverse->next;
			free(traverse->value);
			free(traverse);
			traverse = next;
		}
		free(traverse->value);
		free(traverse);
		return 1;
	}
	else {
		return 1;
	}
}

int freeNodeList(NodeList* head) {
	if (head != NULL) {
		NodeList* traverse = head;
		while (traverse->next != NULL) {
			NodeList* next = traverse->next;
			freeNode(traverse->value);
			free(traverse);
			traverse = next;
		}
		freeNode(traverse->value);
		free(traverse);
		return 1;
	}
	else {
		return 1;
	}
}

NodeList* nodeListInsert(NodeList* head, Node* val) {
	if (head != NULL) {
		NodeList* ptr = head;
		while (ptr->next != NULL) {
			ptr = ptr->next;
		}
		ptr->next = createNodeList(ptr, val, NULL);
	}
	else {
		head = createNodeList(NULL, val, NULL);
	}
	return head;
}

Node* tokenFinder(char* expression, char delimiter) {
	int tokenStart = 0;
	Node* head = NULL;
	int i = -1;
	do {
		i++;
		if ((expression[i] == delimiter) || (expression[i] == '\0')) {
			head = insert(head, stringCopy((expression + tokenStart), (i - tokenStart)));
			tokenStart = i + 1;
		}
	} while (expression[i] != '\0');
	free(expression);
	return head;
}

NodeList* expressionFinder(char* expressions, char delimiter) {
	int expressionStart = 0;
	NodeList* head = NULL;
	int i = -1;
	do {
		i++;
		if ((expressions[i] == delimiter) || (expressions[i] == '\0')) {
			head = nodeListInsert(head, tokenFinder(stringCopy((expressions + expressionStart), (i - expressionStart)), ' '));
			expressionStart = i + 1;
		}
	} while (expressions[i] != '\0');
	return head;
}

int tokenClassifier(char* token, StringArray arithmeticOperators, StringArray logicalOperators, StringArray arithmeticOperands, StringArray logicalOperands) {
	if (stringEquals(token, "NOT")) {
		return LOGICAL_NOT;
	}
	else if (stringArrayContains(arithmeticOperators, token)) {
		return ARITHMETIC_OPERATOR;
	}
	else if (stringArrayContains(logicalOperators, token)) {
		return LOGICAL_OPERATOR;
	}
	else if (stringArrayContains(arithmeticOperands, token)) {
		return ARITHMETIC_OPERAND;
	}
	else if (stringArrayContains(logicalOperands, token)) {
		return LOGICAL_OPERAND;
	}
	else {
		return UNKNOWN_TOKEN;
	}
};

int tokenEvaluator(Node* tokens, int expressionNumber, StringArray arithmeticOperators, StringArray logicalOperators, StringArray arithmeticOperands, StringArray logicalOperands) {
	int expressionType = UNKNOWN_EXPRESSION_TYPE;
	int expressionWasEnded = 1;
	int operatorFound = 0;
	int expectedToken = EXPRESSION_START;
	int previousToken = EXPRESSION_START;

	if (tokens != NULL) {
		Node* ptr;
		if (expressionNumber == 0) {
			ptr = tokens;
		}
		else {
			ptr = tokens->next;
		}
		Node* prev = tokens;
		int currentToken;
		if ((expressionNumber != 0) && (!stringEquals(tokens->value, ""))) {
			currentToken = tokenClassifier(tokens->value, arithmeticOperators, logicalOperators, arithmeticOperands, logicalOperands);
			switch (currentToken) {
			case ARITHMETIC_OPERATOR:
				printf("Error: Scan error in expression %d: incomplete expression\n\"%c\"\n", expressionNumber, tokens->value);
				printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, tokens->value);
				expectedToken = TERMINAL_ARITHMETIC_OPERAND;
				expressionType = ARITHMETIC_EXPRESSION;
				operatorFound = 1;
				break;
			case LOGICAL_OPERATOR:
				printf("Error: Scan error in expression %d: incomplete expression\n\"%c\"\n", expressionNumber, tokens->value);
				printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, tokens->value);
				expectedToken = TERMINAL_LOGICAL_OPERAND;
				expressionType = LOGICAL_EXPRESSION;
				operatorFound = 1;
				break;
			case LOGICAL_NOT:
				printf("Error: Scan error in expression %d: incomplete expression\n\"%c\"\n", expressionNumber, tokens->value);
				expectedToken = TERMINAL_LOGICAL_OPERAND;
				expressionType = LOGICAL_EXPRESSION;
				operatorFound = 1;
				break;
			case ARITHMETIC_OPERAND:
				printf("Error: Scan error in expression %d: incomplete expression\n\"%s\"\n", expressionNumber, tokens->value);
				expectedToken = ARITHMETIC_OPERATOR;
				break;
			case LOGICAL_OPERAND:
				printf("Error: Scan error in expression %d: incomplete expression\n\"%s\"\n", expressionNumber, tokens->value);
				expectedToken = LOGICAL_OPERATOR;
				break;
			case UNKNOWN_TOKEN:
				printf("Error: Scan error in expression %d: incomplete expression\n\"%c\"\n", expressionNumber, tokens->value);
				printf("Error: Parse error in expression %d: unknown identifier\n\"%s\"\n", expressionNumber, tokens->value);
				expectedToken = EXPRESSION_START;
				break;
			default:
				printf("\ndev error\n");
				break;
			}
		}
		for (ptr; ptr != NULL; ptr = ptr->next) {
			currentToken = tokenClassifier(ptr->value, arithmeticOperators, logicalOperators, arithmeticOperands, logicalOperands);
			switch (expectedToken) {
			case EXPRESSION_START:
				switch (currentToken) {
				case ARITHMETIC_OPERATOR:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_ARITHMETIC_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = ARITHMETIC_EXPRESSION;
					}
					break;
				case LOGICAL_OPERATOR:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = LOGICAL_EXPRESSION;
					}
					break;
				case LOGICAL_NOT:
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = LOGICAL_EXPRESSION;
					}
					break;
				case ARITHMETIC_OPERAND:
					expectedToken = ARITHMETIC_OPERATOR;
					break;
				case LOGICAL_OPERAND:
					expectedToken = LOGICAL_OPERATOR;
					break;
				case UNKNOWN_TOKEN:
					printf("Error: Parse error in expression %d: unknown identifier\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = EXPRESSION_START;
					break;
				default:
					printf("\ndev error\n");
					break;
				}
				break;
			case ARITHMETIC_OPERATOR:
				switch (currentToken) {
				case ARITHMETIC_OPERATOR:
					expectedToken = TERMINAL_ARITHMETIC_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = ARITHMETIC_EXPRESSION;
					}
					break;
				case LOGICAL_OPERATOR:
					printf("Error: Parse error in expression %d: type mismatch\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = LOGICAL_EXPRESSION;
					}
					break;
				case LOGICAL_NOT:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = LOGICAL_EXPRESSION;
					}
					break;
				case ARITHMETIC_OPERAND:
					printf("Error: Parse error in expression %d: unexpected operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = ARITHMETIC_OPERATOR;
					break;
				case LOGICAL_OPERAND:
					printf("Error: Parse error in expression %d: unexpected operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = LOGICAL_OPERATOR;
					break;
				case UNKNOWN_TOKEN:
					printf("Error: Parse error in expression %d: unknown operator\n\"%s\"\n", expressionNumber, ptr->value);
					operatorFound = 1;
					expectedToken = TERMINAL_ARITHMETIC_OPERAND;
					break;
				default:
					printf("\ndev error\n");
					break;
				}
				break;
			case LOGICAL_OPERATOR:
				switch (currentToken) {
				case ARITHMETIC_OPERATOR:
					printf("Error: Parse error in expression %d: type mismatch\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_ARITHMETIC_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = ARITHMETIC_EXPRESSION;
					}
					break;
				case LOGICAL_OPERATOR:
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = LOGICAL_EXPRESSION;
					}
					break;
				case LOGICAL_NOT:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					if (expressionType == UNKNOWN_EXPRESSION_TYPE) {
						expressionType = LOGICAL_EXPRESSION;
					}
					break;
				case ARITHMETIC_OPERAND:
					printf("Error: Parse error in expression %d: unexpected operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = ARITHMETIC_OPERATOR;
					break;
				case LOGICAL_OPERAND:
					printf("Error: Parse error in expression %d: unexpected operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = LOGICAL_OPERATOR;
					break;
				case UNKNOWN_TOKEN:
					printf("Error: Parse error in expression %d: unknown operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					break;
				default:
					printf("\ndev error\n");
					break;
				}
				break;
			case TERMINAL_LOGICAL_OPERAND:
				switch (currentToken) {
				case ARITHMETIC_OPERATOR:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_ARITHMETIC_OPERAND;
					operatorFound = 1;
					break;
				case LOGICAL_OPERATOR:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					break;
				case LOGICAL_NOT:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					break;
				case ARITHMETIC_OPERAND:
					if (previousToken != UNKNOWN_TOKEN) {
						printf("Error: Parse error in expression %d: type mismatch\n\"%s\"\n", expressionNumber, ptr->value);
					}
					expectedToken = EXPRESSION_FINISHED;
					break;
				case LOGICAL_OPERAND:
					expectedToken = EXPRESSION_FINISHED;
					break;
				case UNKNOWN_TOKEN:
					printf("Error: Parse error in expression %d: unknown operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = EXPRESSION_FINISHED;
					break;
				default:
					printf("\ndev error\n");
					break;
				}
				break;
			case TERMINAL_ARITHMETIC_OPERAND:
				switch (currentToken) {
				case ARITHMETIC_OPERATOR:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_ARITHMETIC_OPERAND;
					operatorFound = 1;
					break;
				case LOGICAL_OPERATOR:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					break;
				case LOGICAL_NOT:
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					operatorFound = 1;
					break;
				case ARITHMETIC_OPERAND:
					expectedToken = EXPRESSION_FINISHED;
					break;
				case LOGICAL_OPERAND:
					if (previousToken != UNKNOWN_TOKEN) {
						printf("Error: Parse error in expression %d: type mismatch\n\"%s\"\n", expressionNumber, ptr->value);
					}
					expectedToken = EXPRESSION_FINISHED;
					break;
				case UNKNOWN_TOKEN:
					printf("Error: Parse error in expression %d: unknown operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = EXPRESSION_FINISHED;
					break;
				default:
					printf("\ndev error\n");
					break;
				}
				break;
			case EXPRESSION_FINISHED:
				switch (currentToken) {
				case ARITHMETIC_OPERATOR:
					if (expressionWasEnded) {
						printf("Error: Parse error in expression %d: expression wasn't ended\n\"%s\"\n", expressionNumber, ptr->value);
						expressionWasEnded = 0;
					}
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_ARITHMETIC_OPERAND;
					break;
				case LOGICAL_OPERATOR:
					if (expressionWasEnded) {
						printf("Error: Parse error in expression %d: expression wasn't ended\n\"%s\"\n", expressionNumber, ptr->value);
						expressionWasEnded = 0;
					}
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					break;
				case LOGICAL_NOT:
					if (expressionWasEnded) {
						printf("Error: Parse error in expression %d: expression wasn't ended\n\"%s\"\n", expressionNumber, ptr->value);
						expressionWasEnded = 0;
					}
					printf("Error: Parse error in expression %d: unexpected operator\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = TERMINAL_LOGICAL_OPERAND;
					break;
				case ARITHMETIC_OPERAND:
					if (expressionWasEnded) {
						printf("Error: Parse error in expression %d: expression wasn't ended\n\"%s\"\n", expressionNumber, ptr->value);
						expressionWasEnded = 0;
					}
					printf("Error: Parse error in expression %d: unexpected operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = ARITHMETIC_OPERATOR;
					break;
				case LOGICAL_OPERAND:
					if (expressionWasEnded) {
						printf("Error: Parse error in expression %d: expression wasn't ended\n\"%s\"\n", expressionNumber, ptr->value);
						expressionWasEnded = 0;
					}
					printf("Error: Parse error in expression %d: unexpected operand\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = LOGICAL_OPERATOR;
					break;
				case UNKNOWN_TOKEN:
					if (expressionWasEnded) {
						printf("Error: Parse error in expression %d: expression wasn't ended\n\"%s\"\n", expressionNumber, ptr->value);
						expressionWasEnded = 0;
					}
					printf("Error: Parse error in expression %d: unknown identifier\n\"%s\"\n", expressionNumber, ptr->value);
					expectedToken = EXPRESSION_START;
					break;
				default:
					printf("\ndev error\n");
					break;
				}
				break;

			default:
				printf("\ndev error\n");
				break;
			}
			previousToken = currentToken;
			prev = ptr;
		}
		if (expectedToken != EXPRESSION_FINISHED) {
			switch (expectedToken) {
			case EXPRESSION_START:
				if ((expressionType == UNKNOWN_EXPRESSION_TYPE) && (!operatorFound)) {
					printf("Error: Scan error in expression %d: incomplete expression\n\"%s\"\n", expressionNumber, prev->value);
				}
				break;
			case ARITHMETIC_OPERATOR:
				if ((expressionType == UNKNOWN_EXPRESSION_TYPE) && (!operatorFound)) {
					printf("Error: Scan error in expression %d: missing operator\n\"%s\"\n", expressionNumber, prev->value);
				}
				break;
			case LOGICAL_OPERATOR:
				if ((expressionType == UNKNOWN_EXPRESSION_TYPE) && (!operatorFound)) {
					printf("Error: Scan error in expression %d: missing operator\n\"%s\"\n", expressionNumber, prev->value);
				}
				break;
			case TERMINAL_LOGICAL_OPERAND:
				if (expressionWasEnded) {
					printf("Error: Scan error in expression %d: missing operand\n\"%s\"\n", expressionNumber, prev->value);
				}
				break;
			case TERMINAL_ARITHMETIC_OPERAND:
				if (expressionWasEnded) {
					printf("Error: Scan error in expression %d: missing operand\n\"%s\"\n", expressionNumber, prev->value);
				}
				break;
			default:
				printf("\ndev error\n");
				break;
			}
		}
	}
	else {
		printf("\nNull expression in expression %d\n", expressionNumber);
	}
	return expressionType;
}



int main(int argc, char** argv) {
	if (argc != 2) {
		printf("incorrect number of arguments\n");
		return 0;
	}

	StringArray arithmeticOperators;
	char* arithOperatorArray[4] = { "-", "+", "*", "/" };
	arithmeticOperators.array = arithOperatorArray;
	arithmeticOperators.length = 4;

	StringArray logicalOperators;
	char* logicOperatorArray[2] = { "AND", "OR" };
	logicalOperators.array = logicOperatorArray;
	logicalOperators.length = 2;

	StringArray logicalOperands;
	char* logicOperandArray[2] = { "true", "false" };
	logicalOperands.array = logicOperandArray;
	logicalOperands.length = 2;

	StringArray arithmeticOperands;
	char* arithOperandArray[10] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" };
	arithmeticOperands.array = arithOperandArray;
	arithmeticOperands.length = 10;

	int expressionCount = 0;
	int logicalExpressionCount = 0;
	int arithmeticExpressionCount = 0;

	NodeList* expressions = expressionFinder(argv[1], ';');

	NodeList* ptr;
	for (ptr = expressions; ptr != NULL; ptr = ptr->next) {
		int expressionType = tokenEvaluator(ptr->value, expressionCount, arithmeticOperators, logicalOperators, arithmeticOperands, logicalOperands);
		if (expressionType == LOGICAL_EXPRESSION) {
			logicalExpressionCount++;
		}
		else if (expressionType == ARITHMETIC_EXPRESSION) {
			arithmeticExpressionCount++;
		}
		expressionCount++;
	}
	printf("Found %d expressions: %d logical and %d arithmetic.\n", expressionCount, logicalExpressionCount, arithmeticExpressionCount);
	freeNodeList(expressions);
	return 0;
}