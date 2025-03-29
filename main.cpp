#include<bits/stdc++.h>
using namespace std;

void print_prompt () {
    cout << "silly % ";
}

void read_input (string *input_buffer) {
    getline(cin, *input_buffer);
}

void close_buffer (string *input_buffer) {
    free(input_buffer);
}

int main (int argc, char** argv) {
    string *input_buffer = new string();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (*input_buffer == "silly exit") {
            close_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        } else {
            cerr << "Unrecognised command: " << *input_buffer << endl;
            exit(EXIT_FAILURE);
        }
    }
}