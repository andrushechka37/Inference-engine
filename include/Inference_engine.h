#include <cstddef>

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