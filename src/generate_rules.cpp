#include "../include/Inference_engine.h"



int main() {
    table_of_names table = {};
    buffer data = {};
    token_data TOKENS = {};

    read_file_to_buffer("input.txt", &data);
    parse_str_lexically(&data, &table, &TOKENS);

    write_rules(&TOKENS);
}

// g++ -g -c src/generate_rules.cpp -o generate_rules.o && g++ generate_rules.o -o rules && ./rules