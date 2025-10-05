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
    