// Samar Joshi
// GitHub:
// importing library
//Standard input output library
#include <stdio.h>
//Standard library function for memory allocation
#include <stdlib.h>
// importing isdigit for character type functions
#include <ctype.h>
//importing string library for string manipulation
#include <string.h>

// There are 3 main parts of the code,Tokenization,parsing and Evaluation
// Tokenization: Breaking the input into tokens
//Firstly we will enumearate the types of tokens we will be using
typedef enum {
    TOKEN_NUMBER,TOKEN_PLUS,TOKEN_MINUS,TOKEN_MULTIPLY,TOKEN_END} TokenType;

    //Now i will create a structure to represent the token
    typedef struct {
        TokenType type;
        int value;
    } Token;
    //Creating a function to tokenize the input string
    Token * tokenize(const char * input){
    Token *tokens = malloc(256 * sizeof(Token));
    int pos = 0;
    // now i am going to loop through the input string
    while (*input){
        if (isdigit((*input))){
            tokens[pos].type = TOKEN_NUMBER;
    //now i am going to convert the string to long data type
            tokens[pos].value = strtol(input, (char**)&input,10);
            pos++;
            
        }else if (*input == '+'){
            tokens[pos].type = TOKEN_PLUS;
            input++;
            pos++;

        }else if (*input == '-'){
            tokens[pos].type = TOKEN_MINUS;
            input++;
            pos++;

    }else if (*input == '*'){
            tokens[pos].type = TOKEN_MULTIPLY;
            input++;
            pos++;
}else{
    // in this i will skip like other character spaces
    input++;
}
    }
// now i will mark the end of the token
tokens[pos].type = TOKEN_END;
return tokens;
    }
/*  
Second Component of this will be Parsing
The function of the parser is to mainly parse the tokens and evaluate the expression
*/
int parse(Token *tokens, int *result) {
    int pos = 0; //A
    if (tokens[pos].type != TOKEN_NUMBER) {
        return 0; // In unix based systems, which are written in c there are different exit codes like 0,1,2 etc but in here retunning 0 means failure
    }
    *result = tokens[pos].value;
    pos++;
    // Now i will loop through the tokens and will evaulate the expression
    while (tokens[pos].type != TOKEN_END) {
        if (tokens[pos].type == TOKEN_PLUS) {
            pos++;
            if (tokens[pos].type != TOKEN_NUMBER) {
                return 0; //again returning 0 for failure
            }
            *result += tokens[pos].value;
        }else if (tokens[pos].type == TOKEN_MINUS) {
            pos++;
            if (tokens[pos].type != TOKEN_NUMBER) {
                return 0; //again returning 0 for failure
            }
            *result -= tokens[pos].value;
        }else if (tokens[pos].type == TOKEN_MULTIPLY) {
            pos++;
            if (tokens[pos].type != TOKEN_NUMBER) {
                return 0; //again returning 0 for failure
            }
            else{
                return 0; //again returning 0 for failure
            }
            pos++;
        }
    }
    return 1; // returning 1 due to successful parsing
}
/*
Finally our third component is Evaluation
Code generator definition (Assembly like instructions)
*/
void generate_assembly(Token *tokens){}