#include "common/types.hpp"
#include "database/database.hpp"
#include "query/parser.hpp"
#include "io/input_output.hpp"

int main(int argc, char** argv) {
    if (argc >= 2) {
        cout << "Unrequired arguments passed";
        exit(EXIT_FAILURE);
    }
    
    string *input_buffer = new string();
    Database db = Database("");
    
    while(true) {
        printPrompt(db.getName());
        readInput(input_buffer);

        SillyResults result = parseStatement(input_buffer, db);
        
        switch(result) {
            case SillyResults::SILLY_SUCCESS:
                cout << "Executed successfully!" << endl;
                break;
            case SillyResults::SILLY_SYNTAX_ERROR:
                cout << "[Error] Wrong Syntax: [" << *input_buffer << "]" << endl;
                break;
            case SillyResults::SILLY_SEMICOLON_MISSING_ERROR:
                cout << "[Error] Semicolon Missing: [" << *input_buffer << "]" << endl;
                break;
            case SillyResults::SILLY_TYPE_MISMATCH_ERROR:
                cout << "[Error] Type Mismatch: [" << *input_buffer << "]" << endl;
                break;
            case SillyResults::SILLY_TABLE_NOT_FOUND_ERROR:
                cout << "[Error] Table Not Found: [" << *input_buffer << "]" << endl;
                break;
            case SillyResults::SILLY_EXECUTION_ERROR:
                cout << "[Error] Execution Failed: [" << *input_buffer << "]" << endl;
                break;
            case SillyResults::SILLY_UNRECOGNIZED_STATEMENT:
                cout << "[Error] Unrecognized Statement: [" << *input_buffer << "]" << endl;
                break;
            default:
                cout << "Something went wrong :/" << endl;
                break;
        }
    }
}