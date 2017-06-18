/**
 * Kevin Zhang (zhangk6)
 * Homework 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

/**
 * expression struct
 */
typedef struct {
	char operation;
	int value;
} expression;

// function prototypes
int is_op(char);
expression *evaluate(const char *input);

int main(int argc, char* argv[]){
	if(argc != 2) { // check arg count
		printf("ERROR: Invalid arguments\n");
		printf("USAGE: ./a.out <input-file>\n");
		return -1;
	}

	// read input
	FILE *inf;
	if((inf = fopen(argv[1], "r")) != NULL) {
		// get expression line only
		char *input = NULL; // free this!
		size_t n = 0;
		int done = 0;
		while(!done) {
			if(getline(&input, &n, inf) != -1) {
				// ignore comments and newlines
				if(strlen(input) < 1 || input[0] == '#') continue;
				else done = 1;
			} else {
				perror("getline() failed");
				return EXIT_FAILURE;
			}
		}

		// evaluate expression
		expression* expr = evaluate(input);

		// print the answer if it exists (finished evaluate call)
		if(expr) printf("PROCESS %i: Final answer is '%i'\n", getpid(), expr->value);

		// free and exit
		free(expr);
		free(input);
		fclose(inf);
		return EXIT_SUCCESS;
	} else {
		perror("fopen() failed");
		return EXIT_FAILURE;
	}
}

/**
 * returns true if operation and false otherwise
 */
int is_op(char c) {
	switch(c) {
		case '+': case '-': case '*': case '/':
			return 1;
		default:
			return 0;
	}
}

/**
 * evaluates an expression. returns the expression struct (including solution) or NULL if failure.
 */
expression *evaluate(const char* expr) {
	int expr_len = strlen(expr); // expression length
	int idx = 0; // current expression index
	int num_operands = 0; // number of operands read in currently
	expression* res = malloc(sizeof(*res)); // result of current expression

	while(expr[idx] == ' ') idx++; // ignore opening whitespace
	if(expr[idx] == '(') {
		++idx;

		// get operation
		if(is_op(expr[idx]) && expr[idx+1] == ' ') {
			res->operation = expr[idx];
			printf("PROCESS %i: Starting '%c' operation\n", getpid(), expr[idx]);
			if(expr[idx] == '+' || expr[idx] == '-')
				res->value = 0;
			else res->value = 1;
			idx += 2;
		}

		// invalid operator
		else {
			free(res);
			printf("PROCESS %i: ERROR: unknown '", getpid());
			while(expr[idx] != ' ') {
				printf("%c", expr[idx]);
				++idx;
			}
			printf("' operator\n");
			return NULL;
		}

		// iterate through operands and evaluate
		while(idx < expr_len-1) {
			num_operands++;
			while(expr[idx] == ' ') ++idx;
			int end = idx;
			int pid_p;

			// if its a number, grab the entire number
			if(isdigit(expr[idx])) {
				while(isdigit(expr[end])) ++end;
			}

			// otherwise handle the expression
			else if(expr[idx] == '(') {
				// find the closing parentheses
				int n = 1; // once this reaches 0 we've found its partner
				while(n > 0) {
					++end;
					if(expr[end] == ')') --n;
					else if(expr[end] == '(') ++n;
				}
			}

			// finally, return result if we've reached the end
			else if(expr[idx] == ')') {
				return res;
			}

			// last minute failsafe
			else {
				printf("PROCESS %i: ERROR: unknown operand '%c'\n", getpid(), expr[idx]);
				free(res);
				return NULL;
			}

			// begin forking!
			int p[2];
			if(pipe(p) == -1) {
				perror("pipe() failed");
				free(res);
				return NULL;
			}

			char* forked_expr = calloc((end - idx + 1), (sizeof(char)));
			memcpy(forked_expr, &expr[idx], end - idx + 1);

			// child
			if((pid_p = fork()) == 0) {
				close(p[0]);

				expression* new_expr = evaluate(forked_expr);
				free(forked_expr);
				if(!new_expr) {
					free(res);
					return NULL;
				}

				printf("PROCESS %i: Sending '%i' on pipe to parent\n", getpid(), (new_expr->value));
				write(p[1], &(new_expr->value), sizeof(new_expr->value));
				free(new_expr); free(res);
				close(p[1]);
				return NULL;
			}

			// parent
			else if(pid_p > 0) {
				close(p[1]);
				free(forked_expr);

				// wait for child
				int status;
				pid_t child_pid = wait(&status);

				// abnormal termination
				if(WIFSIGNALED(status)) printf("PARENT: child %d terminated abnormally\n", (int) child_pid);

				// other return code
				else if(WIFEXITED(status)) {
					int code = WEXITSTATUS(status);
					if(code != 0) printf("PARENT: child %d terminated with nonzero exit status %d\n", (int) child_pid, code);
				}

				int value;
				read(p[0], &value, sizeof(value));
				close(p[0]);

				// handle math
				switch(res->operation) {
					case '+':
						res->value += value;
						break;
					case '-':
						// first operand for subtraction matters
						if(num_operands == 1) res->value += value;
						else res->value -= value;
						break;
					case '*':
						res->value *= value;
						break;
					case '/':
						// first operand for division matters
						if(num_operands == 1) res->value *= value;
						else res->value /= value;
						break;
				}
			}

			else {
				free(res);
				free(forked_expr);
				perror("ERROR: fork() failed\n");
				return NULL;
			}

			idx = end + 1;
		}

		if(num_operands < 2) {
			printf("PROCESS %i: ERROR: not enough operands\n", getpid());
			free(res);
			return NULL;
		}
	}

	else {
		res->operation = ' ';
		res->value = atoi(expr);
	}

	return res;
}