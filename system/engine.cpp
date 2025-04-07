#include "imports.hpp"
#include "constants.hpp"
#include "row.hpp"
#include "helpers.hpp"
#include "connectors.hpp"
#include "parser.hpp"

// need to do pager and stuffs

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Enter a database filename" << endl;
        exit(EXIT_FAILURE);
    }
    char* filename = argv[1];

    string *input_buffer = new string();
    Schema schema;
    while(true) {
        print_prompt();
        read_input(input_buffer);

        PrepareResult prepareResult = prepare_statement(input_buffer, schema);
        if(prepareResult == PrepareResult::PREPARE_SUCCESS) {
            cout << "Executed successfully!" << endl;
        } else if (prepareResult == PrepareResult::PREPARE_SYNTAX_ERROR) {
            cout << "Syntax Error: " << *input_buffer << endl;
        } else {
            cout << "Something went wrong :/" << endl;
        }
    }
}