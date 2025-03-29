#include<bits/stdc++.h>
using namespace std;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;


typedef enum { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
} StatementType;

typedef struct {
    StatementType type;
} Statement;

void print_prompt () {
    cout << "silly % ";
}

void read_input (string *input_buffer) {
    getline(cin, *input_buffer);
}

void close_buffer (string *input_buffer) {
    free(input_buffer);
}

MetaCommandResult do_meta_command (string *input_buffer) {
    if (*input_buffer == "silly exit") {
        return META_COMMAND_SUCCESS;
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_statement (string *input_buffer, Statement *statement) {
    if (input_buffer->substr(6, 6) == "insert") {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (input_buffer->substr(6, 6) == "select") {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement (Statement* statement) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            cout << "This is where we would do an insert." << endl;
            break;
        case (STATEMENT_SELECT):
            cout << "This is where we would do a select." << endl;
            break;
    }
}

int main (int argc, char** argv) {
    string *input_buffer = new string();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->substr(0, input_buffer->find(" ")) == "silly") {
            switch (do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    break;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    cout << "Unrecognised command: " << *input_buffer << endl;
                    continue;
            }
        }
        
        Statement statement;    
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                cout << "Unrecognised keyword at start: " << *input_buffer << endl;
                continue;
        }

        execute_statement(&statement);
        cout << "Executed" << endl;
    }
}