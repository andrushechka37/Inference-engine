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
// опверка в get var
// добавить дефайнов
// freeeee
// const d snpintf
// include происходит раньше заполнения его
static int get_size_of_file(FILE * file);
static void get_word(buffer * data, char * name);

void var_elem_ctor(bool value, variables_data_elem * elem) 
{
    elem->value = value;
}

int put_name_in_table(table_of_names * table, int number, char * name) 
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

    data->len = get_size_of_file(pfile);
    data->buffer = (const char *) calloc(data->len, sizeof(char));
    fread((void *)(data->buffer), sizeof(char), data->len, pfile);

    fclose(pfile);
}

void create_token(types_of_tokens type, bool value, token * token) 
{
    if (type == number) {
        token->value = value;
    }

    token->type = type;
}

#define CUR_CHAR data->buffer[data->ip]
#define CASE_FOR_SYMBOL(symbol, type)                       \
    case symbol:                                            \
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
            create_token(number, value, &parsed_str->data[parsed_str->ip]);
            data->ip++;
            parsed_str->ip++;

        } else if (isalpha(CUR_CHAR) != 0) {
            char name[MAX_LEN_OF_NAME] = "";
            get_word(data, name);

            int number = put_name_in_table(table, table->len, name);

            create_token(variable_type, 0, &parsed_str->data[parsed_str->ip]);
            parsed_str->data[parsed_str->ip].number = number;
            parsed_str->ip++;

        } else {
            switch (CUR_CHAR) {
                CASE_FOR_SYMBOL('(', bracket_type_o);
                CASE_FOR_SYMBOL(')', bracket_type_c);
                CASE_FOR_SYMBOL('&', conjunction_type);
                CASE_FOR_SYMBOL('|', disjunction_type);
                CASE_FOR_SYMBOL('=', equal);
                CASE_FOR_SYMBOL('!', negation_type);
                CASE_FOR_SYMBOL(';', semicolon);
                CASE_FOR_SYMBOL(',', comma);
                CASE_FOR_SYMBOL('?', question);

                case '-':                                            
                    create_token(delimiter, 0, &parsed_str->data[parsed_str->ip]);    
                    parsed_str->ip_delimiter = parsed_str->ip; 
                    break;

                default:
                    printf("not found symbol %d\n", CUR_CHAR);
            }
            parsed_str->ip++;
            data->ip++;
        }
    }
    create_token(end, 0, &parsed_str->data[parsed_str->ip]);
    parsed_str->ip++;

}

static void get_word(buffer * data, char * name) 
{
    for (int i = 0; ('a' <= CUR_CHAR && CUR_CHAR <= 'z') && (i <= MAX_LEN_OF_NAME); i++) {
        name[i] = CUR_CHAR;
        data->ip++;
    }
}

void get_variables(token_data * parsed_str, variables_data * variables) 
{
    variables->capacity = START_LEN_OF_DATA;
    variables->data = (variables_data_elem *) calloc(START_LEN_OF_DATA, sizeof(variables_data_elem));

    for (int i = 0; parsed_str->data[i].type != delimiter; i++) {

        if (variables->len > variables->capacity) {
            variables->data = (variables_data_elem *) realloc(variables->data, variables->capacity * 2 * sizeof(variables_data_elem));
            variables->capacity *= 2;
        }

        if (parsed_str->data[i].type == variable_type) {
            i++; // cur char =
            i++; // number
            variables->data[variables->len].value = parsed_str->data[i].value;
            i++; // ;
            variables->len++;
        } else {
            printf("token is not variable\n");
        }
    }

    for (int i = 0; i < variables->len; i++) {
        printf("%d\n", variables->data[i].value);
    }
}

void write_rules(token_data * parsed_str) 
{
    FILE * pfile = fopen("rules.h", "w");

    fprintf(pfile, "if ");

    int number_of_rule = 1;
    char message[100] = "";

    for (int i = parsed_str->ip_delimiter + 1; parsed_str->data[i].type != end; i++) {
        switch (parsed_str->data[i].type) {
            case variable_type:
                fprintf(pfile, "variables->data[%zd].value", parsed_str->data[i].number);
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
                snprintf(message, 80, "the rule № %d worked\\n", number_of_rule);

                fprintf(pfile, "\nis_change = 1;\nfprintf(report, \"%s\");\n}\n", message);
                if (parsed_str->data[i + 1].type != end) {
                    fprintf(pfile,"if");
                }
                number_of_rule++;
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
                fprintf(pfile, "%d", parsed_str->data[i].value);
                if (parsed_str->data[i + 1].type != comma) {
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

void save_var(bool * previous_var, variables_data * variables) {
    for (int i = 0; i < variables->len; i++) {
        previous_var[i] = variables->data[i].value;
    }
}

void print_changes(bool * previous_var, variables_data * variables, FILE * report, table_of_names * table) {
    for (int i = 0; i < variables->len; i++) {
        fprintf(report, "%s: %d -> %d\n", table->names_of_var[i].name, previous_var[i], variables->data[i].value);
    }
}

void engine(variables_data * variables, table_of_names * table) {
    bool is_change = 1;
    FILE * report = fopen("report.txt", "w");

    bool * previous_var = (bool *) calloc(variables->len, sizeof(bool));
    while (is_change == 1) {
        is_change = 0;
        save_var(previous_var, variables);
        #include "../rules.h"
        print_changes(previous_var, variables, report, table);
        fprintf(report,"\n------------------\n\n");
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

