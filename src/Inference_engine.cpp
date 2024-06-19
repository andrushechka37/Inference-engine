#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../include/Inference_engine.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// TODO: in table fixed len

// добавить побольше дефайнов посмотреть еще раз либу, посмотреть мейны

static int get_size_of_file(FILE * file);
static void get_word(buffer * data, char * name);

static void var_elem_ctor(bool value, variables_data_elem * elem) 
{
    elem->value = value;
}

static int put_name_in_table(table_of_names * table, int number, char * name) 
{
    for (int i = 0; i < table->len; i++) {
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

    if (pfile == NULL) {
        printf("no input.txt file found\n");
    }

    data->len = get_size_of_file(pfile);
    data->buffer = (const char *) calloc(data->len, sizeof(char));
    fread((void *)(data->buffer), sizeof(char), data->len, pfile);

    fclose(pfile);
}

static void create_token(types_of_tokens type, bool value, token * token) 
{
    if (type == NUMBER_TYPE) {
        token->value = value;
    }

    token->type = type;
}

#define CUR_CHAR data->buffer[data->ip]
#define CUR_TOKEN (parsed_str->data[parsed_str->ip])
#define CASE_FOR_SYMBOL(symbol, type)                             \
    case symbol:                                                  \
    create_token(type, 0, &parsed_str->data[parsed_str->ip]);     \
    break;

void parse_str_lexically(buffer * data, table_of_names * table, token_data * parsed_str) 
{
    parsed_str->data = (token *) calloc(data->len, sizeof(token));

    while (data->ip < data->len) {

        if (isspace(CUR_CHAR) != 0) {
            data->ip++;
            continue;

        } else if (isdigit(CUR_CHAR) != 0) {
            bool value = CUR_CHAR - '0';
            create_token(NUMBER_TYPE, value, &CUR_TOKEN);
            data->ip++;
            parsed_str->ip++;

        } else if (isalpha(CUR_CHAR) != 0) {
            char name[MAX_LEN_OF_NAME] = "";
            get_word(data, name);

            int number = put_name_in_table(table, table->len, name);

            create_token(VARIABLE_TYPE, 0, &CUR_TOKEN);
            CUR_TOKEN.number = number;
            parsed_str->ip++;

        } else {
            switch (CUR_CHAR) {
                CASE_FOR_SYMBOL('(', BRACKET_O_TYPE);
                CASE_FOR_SYMBOL(')', BRACKET_C_TYPE);
                CASE_FOR_SYMBOL('&', CONJUNCTION_TYPE);
                CASE_FOR_SYMBOL('|', DISJUNCTION_TYPE);
                CASE_FOR_SYMBOL('=', EQUAL_TYPE);
                CASE_FOR_SYMBOL('!', NEGATION_TYPE);
                CASE_FOR_SYMBOL(';', SEMICOLON_TYPE);
                CASE_FOR_SYMBOL(',', COMMA_TYPE);
                CASE_FOR_SYMBOL('?', QUESTION_TYPE);

                case '-':                                            
                    create_token(DELIMITER_TYPE, 0, &CUR_TOKEN);    
                    parsed_str->ip_delimiter = parsed_str->ip; 
                    break;

                default:
                    printf("not found symbol %d\n", CUR_CHAR);
            }
            parsed_str->ip++;
            data->ip++;
        }
    }
    create_token(END_TYPE, 0, &CUR_TOKEN);
    parsed_str->ip++;

}

static void get_word(buffer * data, char * name) 
{
    for (int i = 0; ('a' <= CUR_CHAR && CUR_CHAR <= 'z') && (i <= MAX_LEN_OF_NAME); i++) {
        name[i] = CUR_CHAR;
        data->ip++;
    }
}

#define CHECK_SYMBOL(typ)                     \
    i++;                                      \
    if (parsed_str->data[i].type != typ) {    \
        printf("error in get var");           \
    }                                        

void get_variables(token_data * parsed_str, variables_data * variables) 
{
    variables->capacity = START_LEN_OF_DATA;
    variables->data = (variables_data_elem *) calloc(START_LEN_OF_DATA, sizeof(variables_data_elem));

    for (int i = 0; parsed_str->data[i].type != DELIMITER_TYPE; i++) {

        if (variables->len > variables->capacity) {
            variables->data = (variables_data_elem *) realloc(variables->data, variables->capacity * 2 * sizeof(variables_data_elem));
            variables->capacity *= 2;
        }

        if (parsed_str->data[i].type == VARIABLE_TYPE) {
            CHECK_SYMBOL(EQUAL_TYPE);
            CHECK_SYMBOL(NUMBER_TYPE);
            variables->data[variables->len].value = parsed_str->data[i].value;
            CHECK_SYMBOL(SEMICOLON_TYPE);
            variables->len++;
        } else {
            printf("token is not variable\n");
        }
    }

    for (int i = 0; i < variables->len; i++) {
        printf("%d\n", variables->data[i].value);
    }
}

#define PRINT_SYMBOL(typ, symbol)   \
    case typ:                       \
        fprintf(pfile, symbol);     \
        break;

void write_rules(token_data * parsed_str) 
{
    FILE * pfile = fopen("include/rules.h", "w");

    fprintf(pfile, "if ");

    int number_of_rule = 1;
    char message[LEN_OF_MESSAGE] = "";

    for (int i = parsed_str->ip_delimiter + 1; parsed_str->data[i].type != END_TYPE; i++) {
        switch (parsed_str->data[i].type) {
            case VARIABLE_TYPE:
                fprintf(pfile, "variables->data[%zd].value", parsed_str->data[i].number);
                break;

            PRINT_SYMBOL(NEGATION_TYPE, "!");
            PRINT_SYMBOL(CONJUNCTION_TYPE, " && ");
            PRINT_SYMBOL(DISJUNCTION_TYPE, " || ");
            PRINT_SYMBOL(BRACKET_C_TYPE, ")");
            PRINT_SYMBOL(BRACKET_O_TYPE, "(");
            PRINT_SYMBOL(COMMA_TYPE, ";\n");
            PRINT_SYMBOL(QUESTION_TYPE, " {\n");
            PRINT_SYMBOL(EQUAL_TYPE, " = ");

            case SEMICOLON_TYPE:
                snprintf(message, LEN_OF_MESSAGE - 1, "the rule № %d worked\\n", number_of_rule);

                fprintf(pfile, "\nis_change = 1;\nfprintf(report, \"%s\");\n}\n", message);
                if (parsed_str->data[i + 1].type != END_TYPE) {
                    fprintf(pfile,"if");
                }
                number_of_rule++;
                break;  

            case NUMBER_TYPE:
                fprintf(pfile, "%d", parsed_str->data[i].value);
                if (parsed_str->data[i + 1].type != COMMA_TYPE) {
                    fprintf(pfile,";\n");
                }
                break;      

            default:
                fprintf(pfile, "not fount in rules %d\n", parsed_str->data[i].type);
        }

    }
    fclose(pfile);

}

#define TYPES_FOR_LOG(type, text)     \
    case type:                        \
    fprintf(log, text);               \
    break;


static void dump_all_tokens(token_data * TOKENS, table_of_names * table) {
    int i = 0;
    FILE * log = fopen("log.txt", "w");
    while (TOKENS->data[i].type != END_TYPE) {
        switch (TOKENS->data[i].type) {
            TYPES_FOR_LOG(NEGATION_TYPE, "negation\n");
            TYPES_FOR_LOG(CONJUNCTION_TYPE, "conjunction_type\n");
            TYPES_FOR_LOG(DISJUNCTION_TYPE, "disjunction_type\n");
            TYPES_FOR_LOG(BRACKET_C_TYPE, ")\n");
            TYPES_FOR_LOG(EQUAL_TYPE, "=\n");
            TYPES_FOR_LOG(SEMICOLON_TYPE, ";\n");
            TYPES_FOR_LOG(DELIMITER_TYPE, "delimiter\n");
            TYPES_FOR_LOG(END_TYPE, "end\n");
            TYPES_FOR_LOG(BRACKET_O_TYPE, "(\n");
            TYPES_FOR_LOG(QUESTION_TYPE, "?\n");
            TYPES_FOR_LOG(COMMA_TYPE, "comma\n");

            case VARIABLE_TYPE:
                fprintf(log, "var %s, number: %zd\n", table->names_of_var[TOKENS->data[i].number].name, TOKENS->data[i].number);
                break;
            
            case NUMBER_TYPE:
                fprintf(log, "number %d\n", TOKENS->data[i].value);
                break;
        }
        i++;
    }
    fclose(log);
}

static void dump_var(variables_data * variables, FILE * report, table_of_names * table) {
    for (int i = 0; i < variables->len; i++) {
        fprintf(report, "%s = %d\n", table->names_of_var[i].name, variables->data[i].value);
    }
}

static void save_var(bool * previous_var, variables_data * variables) {
    for (int i = 0; i < variables->len; i++) {
        previous_var[i] = variables->data[i].value;
    }
}

static void print_changes(bool * previous_var, variables_data * variables, FILE * report, table_of_names * table) {
    for (int i = 0; i < variables->len; i++) {
        fprintf(report, "%s: %d -> %d\n", table->names_of_var[i].name, previous_var[i], variables->data[i].value);
    }
}

void engine(variables_data * variables, table_of_names * table)
{
    bool is_change = 1;
    FILE * report = fopen("report.txt", "w");

    bool * previous_var = (bool *) calloc(variables->len, sizeof(bool));
    while (is_change == 1) {
        is_change = 0;
        save_var(previous_var, variables);
        #include "../include/rules.h"
        print_changes(previous_var, variables, report, table);
        fprintf(report,"\n------------------\n\n");
    }
    fclose(report);
}





static int get_size_of_file(FILE * file) {

    struct stat buff;
    fstat(fileno(file), &buff);
    return buff.st_size;
}

