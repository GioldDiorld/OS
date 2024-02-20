#include "stdio.h"
#include "string.h"

#define MAX_LEN 100

void count(int data[], int *ptr_data, char operator[], int *ptr_ope);
void calcu(int data[], int *ptr_data, char operator);

int main(int argc, char * argv[])
{
	if (argc != 2) {
		printf("usage: cal {exp}(like: 1+3*4)\n");
		return 0;
	}

	char* exp = argv[1];
	int len = strlen(argv[1]);
	char operator[MAX_LEN];
	operator[0] = '(';
	int ptr_ope = 1;

	int data[MAX_LEN];
	int ptr_data = 0;

	int i, val;
	char ope;

	for (i = 0; i < len; i++) {
		val = 0;
		while(exp[i] <= '9' && exp[i] >= '0')
		{
			val = val * 10 + exp[i] - '0';
			i++;
		}

		if (val) {
			data[ptr_data] = val;
			ptr_data++;
		}

		if (exp[i] == '+' || exp[i] == '-') {
			if (operator[ptr_ope - 1] == '(') {
				operator[ptr_ope] = exp[i];
				ptr_ope++;
			}
			else {
				count(data, &ptr_data, operator, &ptr_ope);
				operator[ptr_ope] = exp[i];
				ptr_ope++;
			}
		}

		else if (exp[i] == '*' || exp[i] == '/') {
			if (operator[ptr_ope - 1] == '*' || operator[ptr_ope - 1] == '/') {
				ope = operator[ptr_ope - 1];
				ptr_ope--;
				calcu(data, &ptr_data, ope);
				operator[ptr_ope] = exp[i];
				ptr_ope++;
			}
			else {
				operator[ptr_ope] = exp[i];
				ptr_ope++;
			}
		}

		else if (exp[i] == '(') {
			operator[ptr_ope] = exp[i];
			ptr_ope++;
		}

		else if (exp[i] == ')') {
			count(data, &ptr_data, operator, &ptr_ope);
			ptr_ope--;
		}
	}

	count(data, &ptr_data, operator, &ptr_ope);
	ptr_ope--;
	printf("%d\n", data[ptr_data - 1]);

	return 0;
}

void count(int data[], int *ptr_data, char operator[], int *ptr_ope) {
	char ope;
	while (operator[*ptr_ope - 1] != '(') {
		ope = operator[*ptr_ope - 1];
		(*ptr_ope)--;
		calcu(data, ptr_data, ope);
	}
}

void calcu(int data[], int *ptr_data, char operator) {
	int ret, d1, d2;
	d2 = data[*ptr_data - 1];
	d1 = data[*ptr_data - 2];
	(*ptr_data) -= 2;

	switch(operator)
	{
		case '+':
			ret = d1 + d2;
			break;
		case '-':
			ret = d1 - d2;
			break;
		case '*':
			ret = d1 * d2;
			break;
		case '/':
			ret = d1 / d2;
			break;
	}

	data[*ptr_data] = ret;
	(*ptr_data)++;

}




