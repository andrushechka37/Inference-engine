#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../include/Inference_engine.h"

// TOD: система ошибок
// TODO: переименовать array
// TODO: все одним сканфом 
// убрать количество аргументов в начале
// убрать костыль с ---------



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
    fscanf(input, "%zd;", &LEN);
    variables->data = (variables_data_elem *) calloc(LEN, sizeof(variables_data_elem));

    int value_of_variable = 0;
    char name_of_var[MAX_LEN_OF_NAME] = "";

    for (int i = 0; fscanf(input, "%s = %d;", name_of_var, &value_of_variable) != 0; i++) {

        if (strcmp(name_of_var, "------") == 0) {    // REMOVE BAD
            break;
        }

        put_name_in_table(table, i, name_of_var);
        var_elem_ctor(value_of_variable, &(variables->data[i]));
    }
}

#undef LEN

int main () 
{
    variables_data variables = {};
    table_of_names table = {};

    FILE * input = fopen("input.txt", "r");
    get_variables(input, &variables, &table);
    fclose(input);


    for (int i = 0; i < variables.len; i++) {
        printf("%s = %d\n", table.names_of_var[i].name, variables.data[i].value);
    }
}