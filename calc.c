// Samar Joshi
// GitHub:https://github.com/Samar2355/Pythonic-Arthemic-Parser-in-C
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
    TOKEN_NUMBER,TOKEN_PLUS,TOKEN_MINUS,TOKEN_MULTIPLY,TOKEN_INVALID,TOKEN_END} TokenType;

    //Now i will create a structure to represent the token
    typedef struct {
        TokenType type;
        int value;
        int start_pos; // 1-based index in original input
    } Token;
    //Creating a function to tokenize the input string
    // Tokenize input and record 1-based start positions for numbers
    Token * tokenize(const char * input){
    Token *tokens = malloc(256 * sizeof(Token));
    if (!tokens) return NULL;
    memset(tokens, 0, 256 * sizeof(Token));
    int pos = 0;
    const char *orig = input;
    // now i am going to loop through the input string
    while (*input){
        // allow signed numbers only at the very start: treat leading '+'/'-' followed by digit as part of number
        if (((*input == '+' || *input == '-') && isdigit((unsigned char)input[1]) && pos == 0) ||
            isdigit((unsigned char)*input)){
            tokens[pos].type = TOKEN_NUMBER;
            tokens[pos].start_pos = (int)(input - orig) + 1;
            // now convert the string to integer
            tokens[pos].value = (int)strtol(input, (char**)&input,10);
            pos++;
            
        }else if (*input == '+'){
            // plus as binary operator
            tokens[pos].type = TOKEN_PLUS;
            tokens[pos].start_pos = (int)(input - orig) + 1;
            input++;
            pos++;

        }else if (*input == '-'){
            // minus as binary operator
            tokens[pos].type = TOKEN_MINUS;
            tokens[pos].start_pos = (int)(input - orig) + 1;
            input++;
            pos++;

    }else if (*input == '*'){
        // multiplication token (may be unsupported by parser in current grade)
        tokens[pos].type = TOKEN_MULTIPLY;
        tokens[pos].start_pos = (int)(input - orig) + 1;
        input++;
        pos++;
    }else if (isspace((unsigned char)*input)){
        // skip spaces
        input++;
    }else{
        // unrecognized character -> invalid token (record its position)
        tokens[pos].type = TOKEN_INVALID;
        tokens[pos].start_pos = (int)(input - orig) + 1;
        input++;
        pos++;
    }
    }
// now i will mark the end of the token
tokens[pos].type = TOKEN_END;
return tokens;
    }
    
// Simple main so the file can be compiled into an executable for debugging
// Read a single-line input from a file (<=100 chars), evaluate, and write result or error to output file
int parse(Token *tokens, int *result, int *error_pos){
    int pos = 0;
    if (tokens[pos].type != TOKEN_NUMBER){
        *error_pos = tokens[pos].start_pos ? tokens[pos].start_pos : 1;
        return 0; // invalid expression
    }
    *result = tokens[pos].value;
    pos++;

    while (tokens[pos].type != TOKEN_END){
        TokenType op = tokens[pos].type;
        int op_pos = tokens[pos].start_pos ? tokens[pos].start_pos : 1;
        if (op != TOKEN_PLUS && op != TOKEN_MINUS){
            *error_pos = op_pos;
            return 0;
        }
        pos++;
        if (tokens[pos].type != TOKEN_NUMBER){
            *error_pos = tokens[pos].start_pos ? tokens[pos].start_pos : op_pos + 1;
            return 0;
        }
        if (op == TOKEN_PLUS) *result += tokens[pos].value;
        else *result -= tokens[pos].value;
        pos++;
    }
    return 1;
}

int main(int argc, char **argv){
    if (argc < 2){
        fprintf(stderr, "Usage: %s input.txt [output.txt]\n", argv[0]);
        return 2;
    }
    const char *infile = argv[1];
    const char *outfile = argc >= 3 ? argv[2] : "output.txt";
// now i will open the input file
    FILE *inf = fopen(infile, "r");
    if (!inf){
        fprintf(stderr, "Could not open input file '%s'\n", infile);
        return 2;
    }
    char buf[128];
    if (!fgets(buf, sizeof(buf), inf)){
        // empty file -> error at position 1
        fclose(inf);
        FILE *outf = fopen(outfile, "w");
        if (outf){
            fprintf(outf, "ERROR:1\n");
            fclose(outf);
        }
        return 0;
    }
    fclose(inf);

    //now i will trim newline
    size_t len = strlen(buf);
    if (len && (buf[len-1] == '\n' || buf[len-1] == '\r')) buf[--len] = '\0';
    //this is to enforce <=100 chars
    if (len > 100) len = 100, buf[100] = '\0';

    Token *tokens = tokenize(buf);
    int result = 0;
    int error_pos = 0;
    int ok = parse(tokens, &result, &error_pos);
// now i will write the output to the file
    FILE *outf = fopen(outfile, "w");
    if (!outf){
        fprintf(stderr, "Could not open output file '%s'\n", outfile);
        free(tokens);
        return 2;
    }
    if (ok){
        fprintf(outf, "%d\n", result);
    }else{
        fprintf(outf, "ERROR:%d\n", error_pos > 0 ? error_pos : 1);
    }
    fclose(outf);
    free(tokens);
    return 0;
}

