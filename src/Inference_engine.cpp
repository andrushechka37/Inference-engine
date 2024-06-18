#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../include/Inference_engine.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// TOD: система ошибок
// TODO: переименовать array и все вообще
// проверка на input
// в таблице имен фиксированное количество
// заполнять деривейт при токенизации



static int get_size_of_file(FILE * file);
static void get_word(buffer * data, char * name);

void var_elem_ctor(bool value, variables_data_elem * elem) 
{
    elem->value = value;
}

int put_name_in_table(table_of_names * table, int number, char * name) 
{
    int i = 0;
    for (i = 0; i < table->len; i++) {
        if (strcmp(name, table->names_of_var[i].name) == 0) {
            return i;
        }
    }
    table->len++;
    strcpy(table->names_of_var[number].name, name); // not safe
    return table->len - 1;
}

void read_file_to_buffer(const char file[], buffer * data) 
{
    FILE * pfile = fopen(file, "r");

    data->len = get_size_of_file(pfile);
    data->buffer = (const char *) calloc(data->len, sizeof(char));
    fread((void *)(data->buffer), sizeof(char), data->len, pfile);

    fclose(pfile);
}

void create_token(types_of_tokens type, bool value, token * TOKEN) {
    if (type == number) {
        TOKEN->value = value;
    }

    TOKEN->type = type;
}

#define CUR_CHAR data->buffer[data->ip]
#define CASE_FOR_SYMBOL(symbol, type)                   \
    case symbol:                                        \
    create_token(type, 0, &TOKENS->data[token_ip]);     \
    break;

void parse_str_lexically(buffer * data, table_of_names * table, token_data * TOKENS) 
{
    TOKENS->data = (token *) calloc(data->len, sizeof(token));
    int token_ip = 0;

    while (data->ip < data->len) {

        if (isspace(CUR_CHAR) != 0) {
            data->ip++;
            continue;

        } else if (isdigit(CUR_CHAR) != 0) {
            bool value = CUR_CHAR - '0';
            create_token(number, value, &TOKENS->data[token_ip]);
            data->ip++;
            token_ip++;

        } else if (isalpha(CUR_CHAR) != 0) {
            char name[MAX_LEN_OF_NAME] = "";
            get_word(data, name);

            int number = put_name_in_table(table, table->len, name);

            create_token(variable_type, 0, &TOKENS->data[token_ip]);
            TOKENS->data[token_ip].number = number;
            token_ip++;

        } else {
            switch (CUR_CHAR) {
                CASE_FOR_SYMBOL('(', bracket_type_o);
                CASE_FOR_SYMBOL(')', bracket_type_c);
                CASE_FOR_SYMBOL('&', conjunction_type);
                CASE_FOR_SYMBOL('|', disjunction_type);
                CASE_FOR_SYMBOL('=', equal);
                CASE_FOR_SYMBOL('!', negation_type);
                CASE_FOR_SYMBOL(';', semicolon);
                CASE_FOR_SYMBOL('-', delimiter);
                CASE_FOR_SYMBOL(',', comma);
                CASE_FOR_SYMBOL('?', question);
                default:
                    printf("not found symbol %d\n", CUR_CHAR);
            }
            token_ip++;
            data->ip++;
        }
    }
    create_token(end, 0, &TOKENS->data[token_ip]);
    token_ip++;

}

static void get_word(buffer * data, char * name) 
{
    int i = 0;
    for (int i = 0; ('a' <= CUR_CHAR && CUR_CHAR <= 'z') && (i <= MAX_LEN_OF_NAME); i++) {
        name[i] = CUR_CHAR;
        data->ip++;
    }
}

void get_variables(token_data * TOKENS, variables_data * variables) {
    variables->capacity = START_LEN_OF_DATA;
    variables->data = (variables_data_elem *) calloc(START_LEN_OF_DATA, sizeof(variables_data_elem));

    int i = 0;
    for (i = 0; TOKENS->data[i].type != delimiter; i++) {
        if (variables->len > variables->capacity) {
            variables->data = (variables_data_elem *) realloc(variables->data, variables->capacity * 2 * sizeof(variables_data_elem));
            variables->capacity *= 2;
        }
        if (TOKENS->data[i].type == variable_type) {
            i++; // cur char =
            i++; // number
            variables->data[variables->len].value = TOKENS->data[i].value;
            i++; // ;
            variables->len++;
        } else {
            printf("token is not variable");
        }
    }
    for (int i = 0; i < variables->len; i++) {
        printf("%d\n", variables->data[i].value);
    }
    TOKENS->ip_delimiter = i;
}

void write_rules(token_data * TOKENS) {
    FILE * pfile = fopen("rules.h", "w");

    fprintf(pfile, "if ");
    int number_of_rule = 1;
    for (int i = TOKENS->ip_delimiter + 1; TOKENS->data[i].type != end; i++) {
        switch (TOKENS->data[i].type) {
            case variable_type:
                fprintf(pfile, "variables->data[%zd].value", TOKENS->data[i].number);
                break;
            case negation_type:
                fprintf(pfile, "!");
                break;

            case conjunction_type:
                fprintf(pfile, " && ");
                break;
            
            case disjunction_type:
                fprintf(pfile, " || ");
                break;     

            case bracket_type_c:
                fprintf(pfile, ")");
                break;  

            case bracket_type_o:
                fprintf(pfile, "(");
                break;         

            case semicolon:
                fprintf(pfile, "\nis_change = 1;\nfprintf(report, \"the rule worked\");\n}\n");
                if (TOKENS->data[i + 1].type != end) {
                    fprintf(pfile,"if");
                }
                break;  

            case comma:
                fprintf(pfile, ";\n");
                break;  

            case question:
                fprintf(pfile, " {\n");
                break; 

            case equal:
                fprintf(pfile, " = ");
                break;          

            case number:
                fprintf(pfile, "%d", TOKENS->data[i].value);
                if (TOKENS->data[i + 1].type != comma) {
                    fprintf(pfile,";\n");
                }
                break;      

            default:
                fprintf(pfile, "not fount in rules %d\n", TOKENS->data[i].type);
        }

    }
    fclose(pfile);

}

#define TYPES_FOR_LOG(type, text)     \
    case type:                        \
    fprintf(log, text);               \
    break;


void dump_all_tokens(token_data * TOKENS, table_of_names * table) {
    int i = 0;
    FILE * log = fopen("log.txt", "w");
    while (TOKENS->data[i].type != end) {
        switch (TOKENS->data[i].type) {
            TYPES_FOR_LOG(negation_type, "negation\n");
            TYPES_FOR_LOG(conjunction_type, "conjunction_type\n");
            TYPES_FOR_LOG(disjunction_type, "disjunction_type\n");
            TYPES_FOR_LOG(bracket_type_c, ")\n");
            TYPES_FOR_LOG(equal, "=\n");
            TYPES_FOR_LOG(semicolon, ";\n");
            TYPES_FOR_LOG(delimiter, "delimiter\n");
            TYPES_FOR_LOG(end, "end\n");
            TYPES_FOR_LOG(bracket_type_o, "(\n");
            TYPES_FOR_LOG(question, "?\n");
            TYPES_FOR_LOG(comma, "comma\n");

            case variable_type:
                fprintf(log, "var %s, number: %zd\n", table->names_of_var[TOKENS->data[i].number].name, TOKENS->data[i].number);
                break;
            
            case number:
                fprintf(log, "number %d\n", TOKENS->data[i].value);
                break;
        }
        i++;
    }
    fclose(log);
}

void dump_var(variables_data * variables, FILE * report, table_of_names * table) {
    for (int i = 0; i < variables->len; i++) {
        fprintf(report, "%s = %d\n", table->names_of_var[i].name, variables->data[i].value);
    }
}

void engine(variables_data * variables, table_of_names * table) {
    bool is_change = 1;
    FILE * report = fopen("report.txt", "w");
    while (is_change == 1) {
        is_change = 0;
        fprintf(report, "then:\n");
        dump_var(variables, report, table);
        #include "../rules.h"
        fprintf(report, "now:\n");
        dump_var(variables, report, table);
    }
    fclose(report);
}

int main () 
{
    variables_data variables = {};
    table_of_names table = {};
    buffer data = {};
    token_data TOKENS = {};
    
    read_file_to_buffer("input.txt", &data);
    parse_str_lexically(&data, &table, &TOKENS);
    dump_all_tokens(&TOKENS, &table);
    get_variables(&TOKENS, &variables);
    write_rules(&TOKENS);

    engine(&variables, &table);

}


static int get_size_of_file(FILE * file) {

    struct stat buff;
    fstat(fileno(file), &buff);
    return buff.st_size;
}



