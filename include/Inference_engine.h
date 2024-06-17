#include <cstddef>
#include <cstdio>

const int MAX_VARIABLES = 10;
const int MAX_LEN_OF_NAME = 10;

struct variables_data_elem {
    bool value;
};


struct variables_data {
    variables_data_elem * data;
    size_t len;
};

struct table_of_names_element {
    char name[MAX_LEN_OF_NAME];
};

struct table_of_names {
    table_of_names_element names_of_var[MAX_VARIABLES];
    size_t len;
};

enum types_of_tokens {
    variable = 1,
    operation = 2
};

struct token {
    types_of_tokens type;
    bool value;
};



bool get_value(char * name, table_of_names * table, variables_data * variables);
bool expression(FILE * pfile, table_of_names * table, variables_data * variables);