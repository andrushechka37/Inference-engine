#include <cstddef>
#include <cstdio>

const int MAX_VARIABLES = 20;
const int MAX_LEN_OF_NAME = 20;
const int START_LEN_OF_DATA = 20;
const int LEN_OF_MESSAGE = 100;

struct variables_data_elem {
    bool value;
};


struct variables_data {
    variables_data_elem * data;
    size_t len;
    size_t capacity;
};

struct table_of_names_element {
    char name[MAX_LEN_OF_NAME];
};

struct table_of_names {
    table_of_names_element names_of_var[MAX_VARIABLES];
    size_t len;
    size_t capacity;
};

enum types_of_tokens {
    VARIABLE_TYPE      =  1,
    NEGATION_TYPE      =  2, 
    CONJUNCTION_TYPE   =  3,
    DISJUNCTION_TYPE   =  4,
    BRACKET_C_TYPE     =  5,
    EQUAL_TYPE         =  6,
    SEMICOLON_TYPE     =  7,
    NUMBER_TYPE        =  8,
    DELIMITER_TYPE     =  9,
    END_TYPE           = 10,
    BRACKET_O_TYPE     = 11,
    QUESTION_TYPE      = 12,
    COMMA_TYPE         = 13,
};

struct token {
    types_of_tokens type;
    bool value;
    size_t number;
};

struct token_data {
    token * data;
    size_t ip;
    size_t ip_delimiter;
};

struct buffer {
    size_t ip;
    const char * buffer;
    size_t len;
};

bool get_value(char * name, table_of_names * table, variables_data * variables);
bool expression(FILE * pfile, table_of_names * table, variables_data * variables);