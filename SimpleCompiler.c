#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_VARIABLES 26
#define TOKEN_NUMBER 1
#define TOKEN_OPERATOR 2
#define TOKEN_IDENTIFIER 3
#define TOKEN_KEYWORD 4
#define TOKEN_UNKNOWN 5

int variables[MAX_VARIABLES];

// Lexical Analyzer
void lexer(char* input, char*** tokens, int* tokenCount) {
    int i = 0, j = 0;
    char currentToken[256];
    *tokenCount = 0;

    *tokens = (char**)malloc(sizeof(char*));

    while (input[i] != '\0') {
        if (isspace(input[i])) {
            i++;
            continue;
        }

        int tokenStart = i;
        int tokenType;

        if (isdigit(input[i])) {
            tokenType = TOKEN_NUMBER;
            while (isdigit(input[i]) && input[i] != '\0') {
                i++;
            }
        } else if (isalpha(input[i])) {
            tokenType = TOKEN_IDENTIFIER;
            while (isalnum(input[i]) && input[i] != '\0') {
                i++;
            }
        } else {
            tokenType = TOKEN_OPERATOR;
            i++;
        }

        int tokenLength = i - tokenStart;
        strncpy(currentToken, &input[tokenStart], tokenLength);
        currentToken[tokenLength] = '\0';

        (*tokens)[j] = (char*)malloc(sizeof(char) * (tokenLength + 1));
        strcpy((*tokens)[j], currentToken);
        (*tokens) = (char**)realloc(*tokens, sizeof(char*) * (j + 2));
        j++;
        (*tokenCount)++;
    }

    (*tokens)[j] = NULL;
}

// Parser
int parse(char** tokens, char* output) {
    int i = 0, j = 0;
    int leftOperand = 0, rightOperand = 0;
    char operator = '+';
    bool isAssignment = false;
    int variableIndex = -1;

    while (tokens[i] != NULL) {
        if (isdigit(tokens[i][0])) {
            if (isAssignment) {
                rightOperand = atoi(tokens[i]);
            } else {
                leftOperand = atoi(tokens[i]);
            }
        } else if (isalpha(tokens[i][0])) {
            if (!isAssignment) {
                variableIndex = tokens[i][0] - 'a';
                leftOperand = variables[variableIndex];
            } else {
                variableIndex = tokens[i][0] - 'a';
            }
        } else if (strcmp(tokens[i], "=") == 0) {
            isAssignment = true;
        } else {
            operator = tokens[i][0];
            if (isAssignment) {
                int result;
                switch (operator) {
                    case '+':
                        result = leftOperand + rightOperand;
                        break;
                    case '-':
                        result = leftOperand - rightOperand;
                        break;
                    case '*':
                        result = leftOperand * rightOperand;
                        break;
                    case '/':
                        if (rightOperand == 0) {
                            printf("Error: Division by zero\n");
                            return 0;
                        }
                        result = leftOperand / rightOperand;
                        break;
                }
                variables[variableIndex] = result;
                isAssignment = false;
                leftOperand = 0;
                rightOperand = 0;
            } else {
                int result;
                switch (operator) {
                    case '+':
                        result = leftOperand + rightOperand;
                        break;
                    case '-':
                        result = leftOperand - rightOperand;
                        break;
                    case '*':
                        result = leftOperand * rightOperand;
                        break;
                    case '/':
                        if (rightOperand == 0) {
                            printf("Error: Division by zero\n");
                            return 0;
                        }
                        result = leftOperand / rightOperand;
                        break;
                }
                leftOperand = result;
                rightOperand = 0;
            }
        }
        i++;
    }

    sprintf(output, "%d", leftOperand);
    return 1;
}

int main() {
    char input[256];
    char** tokens;
    int tokenCount;
    char output[256];

    printf("Enter an expression: ");
    fgets(input, sizeof(input), stdin);

    lexer(input, &tokens, &tokenCount);

    int result = parse(tokens, output);
    if (result) {
        printf("Output: %s\n", output);
    }

    // Free dynamically allocated memory
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);

    return 0;
}
