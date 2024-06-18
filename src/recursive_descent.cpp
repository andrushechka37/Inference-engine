#include "../include/Inference_engine.h"
#include <cstdio>

bool disjunction(FILE * pfile, table_of_names * table, variables_data * variables);

bool get_variable(FILE * pfile, table_of_names * table, variables_data * variables) {
    char var[MAX_LEN_OF_NAME] = "";
    fscanf(pfile, " %s ", var);

    int a = get_value(var, table, variables); // 

    return a;

}

bool negation(FILE * pfile, table_of_names * table, variables_data * variables) {
    if (getc(pfile) == '!') {

        int a = !get_variable(pfile, table, variables);
        return a;
    } else {
        int a = get_variable(pfile, table, variables);
        return a;
    }
}

bool bracket(FILE * pfile, table_of_names * table, variables_data * variables) {
    if (getc(pfile) == '(') {
        bool first = disjunction(pfile, table, variables);

        if (getc(pfile) == '(') {
            return first;
        } else {
            printf("bracket is not closed");
            return first;
        }
    }
    return negation(pfile, table, variables);
}



bool conjunction(FILE * pfile, table_of_names * table, variables_data * variables) {
    bool first = bracket(pfile, table, variables);
    while (getc(pfile) == '&') {
        bool second = bracket(pfile, table, variables);
        first = second && first;
    }
    int a = first;
    return first;
}

bool disjunction(FILE * pfile, table_of_names * table, variables_data * variables) {
    bool first = conjunction(pfile, table, variables);

    while (getc(pfile) == '|') {
        bool second = conjunction(pfile, table, variables);
        first = second || first;
    }
    int a = first;
    return first;
}

bool expression(FILE * pfile, table_of_names * table, variables_data * variables) {
    bool first = disjunction(pfile, table, variables);
    fscanf(pfile, ";");
    return first;
}