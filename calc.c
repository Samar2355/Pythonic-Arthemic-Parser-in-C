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
// including errno and stdint for robust numeric parsing and types
#include <errno.h>
#include <stdint.h>

// There are 3 main parts of the code,Tokenization,parsing and Evaluation
// Tokenization: Breaking the input into tokens
//Firstly we will enumearate the types of tokens we will be using
typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_INVALID,
    TOKEN_END
} TokenType;

    //Now i will create a structure to represent the token
    typedef struct {
        TokenType type;
        long long value;
        int start_pos; // 1-based index in original input
    } Token;
    //Creating a function to tokenize the input string
    // Tokenize input and record 1-based start positions for numbers
    Token * tokenize(char *buf, size_t len, size_t *out_count){
    size_t cap = 64;
    size_t pos = 0;
    Token *tokens = malloc(cap * sizeof(Token));
    if (!tokens) return NULL;
    size_t i = 0;
    // now i am going to loop through the input string
    while (i < len){
        if (pos + 2 > cap){
            size_t ncap = cap * 2;
            Token *t = realloc(tokens, ncap * sizeof(Token));
            if (!t){ free(tokens); return NULL; }
            tokens = t;
            cap = ncap;
        }
        char c = buf[i];
        if (isspace((unsigned char)c)){
            i++; continue;
        }
        // allow signed numbers only at the very start: treat leading '+'/'-' followed by digit as part of number
        if ((isdigit((unsigned char)c)) || ((c == '+' || c == '-') && i == 0 && i + 1 < len && isdigit((unsigned char)buf[i+1]))){
            tokens[pos].type = TOKEN_NUMBER;
            tokens[pos].start_pos = (int)i + 1;
            errno = 0;
            char *endptr = NULL;
            long long val = strtoll(buf + i, &endptr, 10);
            if (endptr == buf + i){
                tokens[pos].type = TOKEN_INVALID;
                tokens[pos].start_pos = (int)i + 1;
                i++; pos++; continue;
            }
            tokens[pos].value = val;
            i = (size_t)(endptr - buf);
            pos++;
        }else if (c == '+'){
            tokens[pos].type = TOKEN_PLUS;
            tokens[pos].start_pos = (int)i + 1;
            i++; pos++;
        }else if (c == '-'){
            tokens[pos].type = TOKEN_MINUS;
            tokens[pos].start_pos = (int)i + 1;
            i++; pos++;
        }else if (c == '*'){
            tokens[pos].type = TOKEN_MULTIPLY;
            tokens[pos].start_pos = (int)i + 1;
            i++; pos++;
        }else if (c == '/'){
            tokens[pos].type = TOKEN_DIVIDE;
            tokens[pos].start_pos = (int)i + 1;
            i++; pos++;
        }else{
            tokens[pos].type = TOKEN_INVALID;
            tokens[pos].start_pos = (int)i + 1;
            i++; pos++;
        }
    }
    // now i will mark the end of the token
    tokens[pos].type = TOKEN_END;
    tokens[pos].start_pos = (int)len + 1;
    pos++;
    *out_count = pos;
    return tokens;
    }
    
// Simple main so the file can be compiled into an executable for debugging

/* Because the original parse signature is used only by main in this file, we'll add a full parser
   using the token array. The wrapper above returns -1 to indicate fallback; main will use the
   full parser implemented below when needed. */

static Token *G_tokens = NULL;
static size_t G_ntokens = 0;
static int G_error_pos = 0;

static int parse_factor(size_t *pidx, long long *out){
    Token *t = &G_tokens[*pidx];
    if (t->type != TOKEN_NUMBER){
        G_error_pos = t->start_pos ? t->start_pos : 1;
        return 0;
    }
    *out = t->value;
    (*pidx)++;
    return 1;
}

static int parse_term(size_t *pidx, long long *out){
    if (!parse_factor(pidx, out)) return 0;
    while (1){
        Token *t = &G_tokens[*pidx];
        if (t->type == TOKEN_MULTIPLY){
            (*pidx)++;
            long long rhs;
            if (!parse_factor(pidx, &rhs)) return 0;
            *out = (*out) * rhs;
        }else if (t->type == TOKEN_DIVIDE){
            int divpos = t->start_pos ? t->start_pos : 1;
            (*pidx)++;
            Token *next = &G_tokens[*pidx];
            if (next->type != TOKEN_NUMBER){
                G_error_pos = next->start_pos ? next->start_pos : divpos + 1;
                return 0;
            }
            if (next->value == 0){
                G_error_pos = next->start_pos ? next->start_pos : divpos;
                return 0;
            }
            long long rhs = next->value;
            (*pidx)++;
            *out = (*out) / rhs;
        }else{
            break;
        }
    }
    return 1;
}

static int parse_expr_full(size_t *pidx, long long *out){
    if (!parse_term(pidx, out)) return 0;
    while (1){
        Token *t = &G_tokens[*pidx];
        if (t->type == TOKEN_PLUS){
            (*pidx)++;
            long long rhs;
            if (!parse_term(pidx, &rhs)) return 0;
            *out = (*out) + rhs;
        }else if (t->type == TOKEN_MINUS){
            (*pidx)++;
            long long rhs;
            if (!parse_term(pidx, &rhs)) return 0;
            *out = (*out) - rhs;
        }else{
            break;
        }
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
    FILE *inf = fopen(infile, "rb");
    if (!inf){
        fprintf(stderr, "Could not open input file '%s'\n", infile);
        return 2;
    }
    if (fseek(inf, 0, SEEK_END) != 0){ fclose(inf); fprintf(stderr, "fseek failed\n"); return 2; }
    long flen = ftell(inf);
    if (flen < 0){ fclose(inf); fprintf(stderr, "ftell failed\n"); return 2; }
    rewind(inf);

    size_t max_allow = 10 * 1024 * 1024;
    if ((size_t)flen > max_allow){ fclose(inf); fprintf(stderr, "Input too large\n"); return 2; }

    size_t buf_len = (size_t)flen;
    char *buf = malloc(buf_len + 1);
    if (!buf){ fclose(inf); fprintf(stderr, "Out of memory\n"); return 2; }

    size_t readn = fread(buf, 1, buf_len, inf);
    fclose(inf);
    if (readn != buf_len){ free(buf); fprintf(stderr, "Failed to read file\n"); return 2; }
    buf[buf_len] = '\0';

    if (buf_len == 0){
        FILE *outf = fopen(outfile, "w");
        if (outf){ fprintf(outf, "ERROR:1\n"); fclose(outf); }
        free(buf);
        return 0;
    }

    Token *tokens = NULL;
    size_t ntokens = 0;
    tokens = tokenize(buf, buf_len, &ntokens);
    if (!tokens){ free(buf); fprintf(stderr, "Tokenization failed (memory)\n"); return 2; }

    for (size_t ti = 0; ti < ntokens; ++ti){
        if (tokens[ti].type == TOKEN_INVALID){
            FILE *outf = fopen(outfile, "w");
            if (outf){ fprintf(outf, "ERROR:%d\n", tokens[ti].start_pos ? tokens[ti].start_pos : 1); fclose(outf); }
            free(tokens); free(buf); return 0;
        }
    }

    G_tokens = tokens; G_ntokens = ntokens; G_error_pos = 0;
    size_t idx = 0;
    long long result = 0;
    int ok = parse_expr_full(&idx, &result);
    if (!ok){
        int errp = G_error_pos ? G_error_pos : 1;
        FILE *outf = fopen(outfile, "w");
        if (outf){ fprintf(outf, "ERROR:%d\n", errp); fclose(outf); }
        free(tokens); free(buf); return 0;
    }

    if (G_tokens[idx].type != TOKEN_END){
        int errp = G_tokens[idx].start_pos ? G_tokens[idx].start_pos : 1;
        FILE *outf = fopen(outfile, "w");
        if (outf){ fprintf(outf, "ERROR:%d\n", errp); fclose(outf); }
        free(tokens); free(buf); return 0;
    }

    FILE *outf = fopen(outfile, "w");
    if (!outf){ free(tokens); free(buf); fprintf(stderr, "Could not open output file '%s'\n", outfile); return 2; }
    fprintf(outf, "%lld\n", result);
    fclose(outf);

    free(tokens);
    free(buf);
    return 0;
// In this, i have used the help of multiple videos and documentations to create this code. although i had to use some of the code from online resources as well as use a bit of help from AI as well.
/*
- https://www.youtube.com/watch?v=0k-c0vvs6HA
- https://www.youtube.com/watch?v=rJrd2QMVbGM&t=1179s
Since i had also used AI assistance, i would like to mention that as well.
My prompt to AI was:
"Help me regarding the error- File not found while compiling C code in VS code" This was asked as i was unable to compile the code, first because i didnt open the developer terminal, and second because the path variable was not set properly.
The AI helped me to set the path variable properly and guided me to open the developer terminal in VS code.
*/
}
