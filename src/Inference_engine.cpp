#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../include/Inference_engine.h"

// TOD: система ошибок
// TODO: переименовать array
// TODO: все одним сканфом 
// проверка на input
// вместо вычисления генерилка кода а то каждый раз заново работать не будет


void var_elem_ctor(bool value, variables_data_elem * elem) 
{
    elem->value = value;
}

void put_name_in_table(table_of_names * table, int number, char * name) 
{
    strcpy(table->names_of_var[number].name, name);
}


#define LEN (variables->len)

void get_variables(FILE * input, variables_data * variables, table_of_names * table) 
{
    variables->capacity = START_LEN_OF_DATA;
    variables->data = (variables_data_elem *) calloc(START_LEN_OF_DATA, sizeof(variables_data_elem));

    int value_of_variable = 0;
    char name_of_var[MAX_LEN_OF_NAME] = "";

    for (int i = 0; fscanf(input, "%s = %d;", name_of_var, &value_of_variable) != 0; i++) {

        if (strcmp(name_of_var, "-") == 0) {
            break;
        }

        (table->len)++;
        (variables->len)++;

        if (variables->len > variables->capacity) {
            variables->data = (variables_data_elem *) realloc(variables->data, variables->capacity * 2 * sizeof(variables_data_elem));
            variables->capacity *= 2;
        }

        put_name_in_table(table, i, name_of_var);
        var_elem_ctor(value_of_variable, &(variables->data[i]));
    }
}

#undef LEN

bool get_value(char * name, table_of_names * table, variables_data * variables) {
    for (int i = 0; i < table->len; i++) {
        if (strcmp(name,table->names_of_var[i].name) == 0) {
            return variables->data[i].value;
        }
    }
    printf("not found name of var\n");
    return 0;
}


void engine(FILE * pfile, table_of_names * table, variables_data * variables) {
    bool is_change = 0;

    while (is_change == 1) {
        if (expression(pfile, table, variables)) {
            // какая то хуйня меняющая всякое, тоже надо обойтись ьез сканфа
        }
    }
}

int main () 
{
    variables_data variables = {};
    table_of_names table = {};
    FILE * input = fopen("input.txt", "r");
    get_variables(input, &variables, &table);

    engine(input, &table, &variables); 

    fclose(input);


    for (int i = 0; i < variables.len; i++) {
        printf("%s = %d\n", table.names_of_var[i].name, variables.data[i].value);
    }
}