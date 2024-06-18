#include <cstddef>
#include <cstdio>

const int MAX_VARIABLES = 10;
const int MAX_LEN_OF_NAME = 10;
const int START_LEN_OF_DATA = 10;

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

enum types_of_tokens { // to caps with one style
    variable_type      = 1,
    negation_type      = 2, 
    conjunction_type   = 3,
    disjunction_type   = 4,
    bracket_type_c      = 5,
    equal              = 6,
    semicolon          = 7,
    number             = 8,
    delimiter          = 9,
    end                = 10,
    bracket_type_o     = 11,
    question           = 12,
    comma              = 13

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