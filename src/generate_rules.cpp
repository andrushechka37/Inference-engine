#include "../include/Inference_engine.h"
#include <cstdlib>



int main() {
    table_of_names table = {};
    buffer data = {};
    token_data TOKENS = {};

    read_file_to_buffer("input.txt", &data);
    parse_str_lexically(&data, &table, &TOKENS);

    write_rules(&TOKENS);

    free((void *)data.buffer);
    free(TOKENS.data);
}