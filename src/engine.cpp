#include "../include/Inference_engine.h"
#include <cstdlib>

int main () 
{
    variables_data variables = {};
    table_of_names table = {};
    buffer data = {};
    token_data TOKENS = {};
    
    read_file_to_buffer("input.txt", &data);
    parse_str_lexically(&data, &table, &TOKENS);
    // dump_all_tokens(&TOKENS, &table);
    get_variables(&TOKENS, &variables);
    write_rules(&TOKENS);

    engine(&variables, &table);



    free(variables.data);           // may be put in dtor function
    free((void *)data.buffer);
    free(TOKENS.data);
}