/**
 * Kevin Zhang (zhangk6)
 * Homework 2
 */

#include <pthread.h>
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

/**
 * arguments for evaluate()
 */
typedef struct {
	const char *expr;
} args;

// function prototypes
int is_op(char);
void *evaluate(void *);

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

		// evaluate expression inside a new thread
		pthread_t expr_t;
		int rc;
		args *arg = (args *)malloc(sizeof(arg));
		arg->expr = input;

		rc = pthread_create(&expr_t, NULL, evaluate, (void *)arg);
		if (rc != 0) {
			perror("pthread_create() failed");
		}

		// release the thread
		void *status;
		pthread_join(expr_t, &status);
		expression *s = (expression *) status;
		printf("%d\n", s->value);
		free(s);

		// free and exit
		free(arg);
		free(input);
		fclose(inf);
		pthread_exit(NULL);
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
 * evaluates an expression.
 * returns the expression struct (including solution) or NULL if failure.
 */
void *evaluate(void *t) {
	//expression *s = (expression *)malloc(sizeof(expression));
	//s->value = 42;
	//pthread_exit((void *)s);
	args *arg_expr = (args *) t;
	const char *expr = arg_expr->expr;
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
			printf("THREAD %u: Starting '%c' operation\n", (int) pthread_self(), expr[idx]);
			if(expr[idx] == '+' || expr[idx] == '-')
				res->value = 0;
			else res->value = 1;
			idx += 2;
		}

		// invalid operator
		else {
			free(res);
			printf("THREAD %u: ERROR: unknown '", (int) pthread_self());
			while(expr[idx] != ' ') {
				printf("%c", expr[idx]);
				++idx;
			}
			printf("' operator\n");
			pthread_exit((void *) NULL);
		}

		// iterate through operands and evaluate
		while(idx < expr_len-1) {
			num_operands++;
			while(expr[idx] == ' ') ++idx;
			int end = idx;

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
				pthread_exit((void *)res);
			}

			// last minute failsafe
			else {
				printf("THREAD %u: ERROR: unknown operand '%c'\n", (int) pthread_self(), expr[idx]);
				free(res);
				return NULL;
			}

			// begin parallelizing threads

			// create joinable threads
			pthread_t expr_t;
			pthread_attr_t attr;
			int rc;
			void *status;
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

			// thread argument
			char *thread_expr = calloc((end - idx + 1), (sizeof(char)));
			memcpy(thread_expr, &expr[idx], end - idx + 1);
			args *arg = (args *)malloc(sizeof(arg));
			arg->expr = thread_expr;

			// begin threading
			rc = pthread_create(&expr_t, &attr, evaluate, (void *)arg);
			if (rc == 0) {
				// release the thread
				pthread_join(expr_t, &status);
				expression *ret = (expression *) status;
				int value = ret->value;
				printf("THREAD %u: Sending '%i' on pipe to parent\n", (int) expr_t, value);

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
					free(arg);
					perror("pthread_create() failed");
					pthread_exit((void *) NULL);
				}

			idx = end + 1;
		}

		if(num_operands < 2) {
			printf("THREAD %u: ERROR: not enough operands\n", (int) pthread_self());
			free(res);
			pthread_exit((void *) NULL);
		}
	}

	else {
		res->operation = ' ';
		res->value = atoi(expr);
	}

	pthread_exit((void *)res);
}