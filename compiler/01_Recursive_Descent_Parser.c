#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

enum {NUL, INT, FLT, PLUS, MINUS, STAR, DIVID, LP, RP, END} token;
typedef struct {
	enum {I, F} t; 
	union {int i; double f;}val;
}RET;

void error(int i);
int warnning = 0;
int get_token();
RET expression();
RET term();
RET factor();
RET number;
char input[100];
int pos = 0;
int p_even = 0;

void error(int i) {
    switch(i) {
        case 1: printf("ERROR : number is wrong\n"); exit(1);
        case 2: printf("ERROR : Invalid expression\n"); exit(1);
        case 3: printf("ERROR : 0 divede\n"); exit(1);
		case 4: printf("ERROR : Unbalanced parentheses\n"); exit(1);
    }
}

int get_token() {
    char ch = input[pos++];
	while (isspace(ch)) ch=input[pos++]; //empty space
	number.t = I;

    int idx=0;
    char s[20];
    char *str;
    str=s;

    if (isdigit(ch)) {
		s[idx++] = ch;
		ch = input[pos++];
		while(ch == '.' || isdigit(ch)) {
			s[idx++] = ch;
			if (ch=='.') number.t = F;
			ch = input[pos++];
		} 
		pos--;
		if(!isdigit(s[idx-1])) error(1); //number 
        if (number.t == F) {
			number.val.f = atof(str);
			return (FLT);
		} else  {
			number.val.i = atoi(str);
			return (INT);
		}
    }
    else if (ch=='+'){return(PLUS);}
    else if (ch=='-'){return(MINUS);}
    else if (ch=='*'){return(STAR);}
    else if (ch=='/'){return(DIVID);}
    else if (ch=='('){p_even++; return(LP);}
    else if (ch==')'){p_even++; return(RP);}
    else if (ch=='\0') return(END);
    else error(2); // wrong input
    return 0;
}

RET factor() {
    RET result;
    if(token==INT || token==FLT) {
        result = number;
        token = get_token();
    }
    else if(token==LP) {
        token = get_token();
        result = expression();
        if(token != RP) error(4); // LP -> RP
		token = get_token();
    } 
    else error(2);
    return(result);
}

RET term() {
    RET result = factor();
    while(token==STAR || token==DIVID) {
        if(token==STAR) {
			token = get_token();
			RET factor_result = factor();
			if(result.t == I) {
				if(factor_result.t == I) result.val.i *= factor_result.val.i;
				else {
					result.t = F;
					result.val.f = result.val.i * factor_result.val.f;
					warnning = 1;
				} 
			} else if (result.t == F){
				if(factor_result.t == F) result.val.f *= factor_result.val.f;
				else {
					result.val.f *= factor_result.val.i;
					warnning = 1;
				}
			}
		} else {
			token = get_token();
			RET factor_result = factor();
			if(factor_result.t == I && factor_result.val.i == 0) error(3); // 0 divide
			if(factor_result.t == F && factor_result.val.f == 0) error(3); // 0 divide
			if(result.t == I) {
				if(factor_result.t == I) result.val.i /= factor_result.val.i;
				else {
					result.t = F;
					result.val.f = result.val.i / factor_result.val.f;
					warnning = 1;
				}
			} else {
				if(factor_result.t == F) result.val.f /= factor_result.val.f;
				else {
					result.val.f /= factor_result.val.i;
					warnning = 1;
				}
			}
		}
    }
    return result;
}

RET expression() {
    RET result = term();
    while(token==PLUS || token==MINUS) {
		if (token == PLUS) {
			token = get_token();
			RET term_result = term();
			if (result.t == I) {
				if(term_result.t == I) result.val.i += term_result.val.i;
				else {
					result.t = F;
					result.val.f = result.val.i + term_result.val.f;
					warnning = 1;
				}
			} else {
				if(term_result.t == F) result.val.f += term_result.val.f;
				else {
					result.val.f += term_result.val.i;
					warnning = 1;
				}
			}
		}	
        else {
			token = get_token();
			RET term_result = term();
			if(result.t == I) {
				if(term_result.t == I) result.val.i -= term_result.val.i;
				else {
					result.t = F;
					result.val.f = result.val.i + term_result.val.f;
					warnning=1;
				}
			} else {
				if(term_result.t == F) result.val.f -= term_result.val.f;
				else {
					result.val.f -= term_result.val.i;
					warnning=1;
				}
			}
		}
	}	
    return result;
}

int main() {
	printf("Enter an expression: ");
	fgets(input, sizeof(input), stdin);
    token = get_token();
    
	RET result = expression();

	if(p_even%2 == 1) error(4);
    if(warnning == 1) printf("WARNNING : Mixing real numbers and integers\n");
	if (result.t == I) printf("Result (integer) : %d\n", result.val.i);
	else printf("Result (float) : %f\n", result.val.f);
    return 0;
}
