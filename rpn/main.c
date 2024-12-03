// Small Reverse Polish notation expression evaluator
// By Ben Jones a.k.a DreamVB
// Date 25/8/24
// Last-Update 20:46 pm

//Thanks to 2kaud for giving me some ideas for fixes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_TOKENS 1024
#define MAX_BUFFER 30
#define MAX_STACK 256
#define MAX_VARS 256
#define MAX_VAR_NAME 30

#define ERR_EMPTY_EXPR 100
#define ERR_UNDERFLOW 101
#define ERR_DIVBY_ZERO 102
#define ERR_UNK_TOKEN 103
#define ERR_OVERFLOW 104


#define PI 3.14159265358979323846

//Variable data
struct TVars{
    char name[MAX_VAR_NAME];
    double value;
};

struct TVars _vars[MAX_VARS];
int var_count = 0;

double stk[MAX_STACK] = {0.0};
int stk_ptr = -1;

void _die(int);
int var_index(char *);
void set_var(char*, double);
double get_var_value(char *);

double rpn_exp(char *);
int is_number(char *);
int is_op(char);
void u_case(char *);
//Maths functions
double sqr(double);
double _max(double, double);
double _min(double, double);
//Stack stuff
int stk_size(void);
double stk_pop();
void stk_push(double);

void _die(int ab_code){
    //Display banner and error message and exit.
    printf("Ben's Reverse Polish Notation Calculator.\n");
    printf("Version 1.4");
    printf("\nError Code: 0x%x\n",ab_code);
    printf("Error: ");

    //Display error message
    switch(ab_code){
        case ERR_EMPTY_EXPR:
            printf("Empty expression.");
            break;
        case ERR_UNDERFLOW:
            printf("Stack underflow.");
            break;
        case ERR_DIVBY_ZERO:
            printf("Division by zero.");
            break;
        case ERR_UNK_TOKEN:
            printf("Unknown token found.");
            break;
        case ERR_OVERFLOW:
            printf("Stack overflow");
            break;
    }

    printf("\n");
    //Play a beep
    printf("%c",7);
    exit(EXIT_FAILURE);
}

int var_index(char *vname){
    //Find the index of a variable in the variable type
    int ndx = -1;
    register int x;

    for(x = 0; x < var_count;x++){
        //Compare two values
        if(strcmp(_vars[x].name,vname) == 0){
            //Variable found set ndx to x
            ndx = x;
            //Break out of loop
            break;
        }
    }
    //Return found index
    return ndx;
}

void set_var(char* vname, double vdata){
    //Adds or sets a variables data
    char temp[MAX_VAR_NAME];
    //We need to copy vname to temp
    strcpy(temp,vname);
    //Upper-case temp
    u_case(temp);
    //Find the variable index
    int ndx = var_index(temp);
    //If error index was found add a new variable and data
    if(ndx == -1){
        //Copy temp to _vars[n].name
        strcpy(_vars[var_count].name,temp);
        //Set the variables data
        _vars[var_count].value = vdata;
        var_count++;
    }else{
        //Editing a variable using existing index
        _vars[ndx].value = vdata;
    }
}

double get_var_value(char *name){
    char temp[MAX_VAR_NAME];
    //Copy name to temp
    strcpy(temp,name);
    //Upper-case temp
    u_case(temp);
    //Find the variable index
    int ndx = var_index(temp);
    return ndx == -1 ?  0.0 : _vars[ndx].value;
}

void stk_push(double value){
    if(stk_ptr >= MAX_STACK - 1){
        _die(ERR_OVERFLOW);
    }
    stk[++stk_ptr] = value;
}

double stk_pop(){
    double value;
    if(stk_ptr < -1){
        _die(ERR_UNDERFLOW);
    }
    value = stk[stk_ptr--];
    return value;
}

int stk_size(){
    return stk_ptr;
}

double sqr(double x){
    return x * x;
}

double _max(double a, double b){
    //Just finds the maximum of two values
    return a > b ? a : b;
}

double _min(double a, double b){
    //Just finds the minimum of two values
    return a < b ? a : b;
}

int is_op(char c){
    //This function return 1 of we have an operator or 0 if not
    return strchr("+-*/%^", c) ? 1 : 0;
}

void u_case(char *s){
    //Upper-case string
    while(*s){
        *s = toupper(*s);
        ++s;
    }
}

int is_number(char *s){
    //Check to see if we have a number.
    char* s0;
    double number;
    number = strtod(s, &s0);
    return s0 == s ? 0 : 1;
}

double rpn_exp(char *expr){
    register int x;
    register int y;
    register int total;
    char buffer[MAX_BUFFER] = {'\0'};
    char tokens[MAX_TOKENS][MAX_BUFFER];
    double a = 0.0;
    double b = 0.0;
    y = 0;
    total = 0;

    //Break up the string into parts breaking were there is a space
    for(x = 0;x < strlen(expr);x++){
        if(isspace(expr[x])){
            //Check buffer length
            if(strlen(buffer) > 0){
                //Copy buffer to tokens array
                strcpy(tokens[total],buffer);
                //INC tokens count
                total++;
                //Reset y to zero
                y = 0;
            }
        }else{
            //Not a space so must be other char so build the buffer string
            buffer[y] = toupper(expr[x]);
            //INC y
            y++;
        }
        //Add ending for buffer
        buffer[y] = '\0';
    }
    //Check if there is anything left in buffer
    if(strlen(buffer) > 0){
        //Copy buffer to tokens array
        strcpy(tokens[total],buffer);
        //INC tokens array count
        total++;
    }
    //Reset x
    x = 0;

    //Check that we have tokens
    if(total < 1){
        _die(ERR_EMPTY_EXPR);
    }
    //Below here we parse the tokens and do the calculations
    for(x = 0; x < total;x++){
        //Check if we have a number
        if(is_number(tokens[x])){
            //Push the number onto the stack
            stk_push(atof(tokens[x]));
        }else{
            //Not a number maybe a operator of function
            //Pop off the two top items on the stack
            if(is_op(tokens[x][0])){
                if(stk_size() < 1){
                    _die(ERR_UNDERFLOW);
                }
                b = stk_pop();
                a = stk_pop();
                //Do the calc and push back the result onto the stack
                switch(tokens[x][0]){
                    case '+':
                        stk_push(a + b);
                        break;
                    case '-':
                        stk_push(a - b);
                        break;
                    case '*':
                        stk_push(a * b);
                        break;
                    case '/':
                        //Need to test for division by zero
                        if(a != 0.0){
                            stk_push(a / b);
                        }
                        else{
                            _die(ERR_DIVBY_ZERO);
                        }
                        break;
                    case '%':
                        //Need to test for division by zero
                        if(a != 0.0){
                            stk_push(fmod(a,b));
                        }else{
                            _die(ERR_DIVBY_ZERO);
                        }
                        break;
                    case '^':
                        stk_push(pow(a,b));
                        break;

                }
            }
            else{
                //Some functions
                if(strcmp(tokens[x],"MIN") == 0){
                    stk_push(_min(stk_pop(),stk_pop()));
                }else if(strcmp(tokens[x],"MAX") == 0){
                    stk_push(_max(stk_pop(),stk_pop()));
                }else if(strcmp(tokens[x],"DUP") == 0){
                    a = stk_pop();
                    stk_push(a);
                    stk_push(a);
                }else if(strcmp(tokens[x],"PI") == 0){
                    stk_push(PI);
                }else if(strcmp(tokens[x],"SQR") == 0){
                    stk_push(sqr(stk_pop()));
                }else if(strcmp(tokens[x],"SQRT") == 0){
                    stk_push(sqrt(stk_pop()));
                }else if(strcmp(tokens[x],"SIN") == 0){
                    stk_push(sin(stk_pop()));
                }else if(strcmp(tokens[x],"COS") == 0){
                    stk_push(cos(stk_pop()));
                }else if(strcmp(tokens[x],"TAN") == 0){
                    stk_push(tan(stk_pop()));
                }else if(strcmp(tokens[x],"ASIN") == 0){
                    stk_push(asin(stk_pop()));
                }else if(strcmp(tokens[x],"ACOS") == 0){
                    stk_push(acos(stk_pop()));
                }else if(strcmp(tokens[x],"ATAN") == 0){
                    stk_push(atan(stk_pop()));
                }else if(strcmp(tokens[x],"EXP") == 0){
                    stk_push(exp(stk_pop()));
                }else if(strcmp(tokens[x],"LN") == 0){
                    stk_push(log(stk_pop()));
                }else if(strcmp(tokens[x],"LOG10") == 0){
                    stk_push(log10(stk_pop()));
                }else{
                    //Must be a variable
                    if(var_index(tokens[x]) != -1){
                        stk_push(get_var_value(tokens[x]));
                    }else{
                        _die(ERR_UNK_TOKEN);
                    }
                }
            }
        }
    }
    //Return the top item of the stack.
    return stk_pop();
}

int main()
{
    set_var("first",20);
    set_var("second",50);

    printf("%lf\n",rpn_exp("10 2 / 5 *"));
    printf("%lf\n",rpn_exp("22 7 /"));
    printf("%lf\n",rpn_exp("5 dup +"));
    printf("%lf\n",rpn_exp("2 4 ^"));
    printf("%lf\n",rpn_exp("10 50 max"));
    printf("%lf\n",rpn_exp("3 2 min"));
    printf("%lf\n",rpn_exp("3 sqr"));
    printf("%lf\n",rpn_exp("20 sqrt"));
    printf("%lf\n",rpn_exp("PI"));
    printf("%lf\n",rpn_exp("first second +"));

    return EXIT_SUCCESS;
}
