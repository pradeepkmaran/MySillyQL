#include "input_output.hpp"

void printPrompt(string dbName) {
    cout << dbName << "/> ";
}

void readInput(string *input_buffer) {
    getline(cin, *input_buffer);
}

void closeInputBuffer(string *input_buffer) {
    delete input_buffer;
}