// #include "imports.hpp"
// #include "constants.hpp"
// #include "helpers.hpp"
// #include "connectors.hpp"
// #include "row.hpp"

// silly make table table_name with columns column1 of integer as primary,column2 of text,column3 of decimal, column4 of boolean;
PrepareResult prepare_create_statement (string *statement, Schema &schema) {
    if (statement->back() != ';') {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }
    vector<string> tokens = split_on(statement, ' ');
    vector<string> fine_tokens;
    vector<string> delimits1, delimits2;
    for (int token_index = 0; token_index < (int32_t)(tokens.size()); token_index++) {
        delimits1 = split_on(&tokens[token_index], ',');
        for (string delimit1: delimits1) {
            vector<string> delimits2 = split_on(&delimit1, ';');
            for (string delimit2: delimits2) {
                fine_tokens.push_back(delimit2);
            }
        }
    }
    tokens = fine_tokens;

    if (tokens[0] != "silly" || tokens[1] != "make" || tokens[2] != "table" || tokens[4] != "with" || tokens[5] != "columns") {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }

    schema = Schema(tokens[3]);

    for (int token_index=6; token_index < (int32_t)(tokens.size()); ) {
        string colName = tokens[token_index++];
        
        if (tokens[token_index++] != "of") return PrepareResult::PREPARE_SYNTAX_ERROR;

        FieldType fieldType = stringToFieldType(tokens[token_index++]);
        bool isPrimary = false;
        if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "as" && token_index++) {
            if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "primary" && token_index++) {
                isPrimary = true;
            }
        }
        schema.addColumn(ColumnDef(colName, fieldType, isPrimary));
    }

    for (ColumnDef col: schema.getColumns()) {
        cout << col.getName() << " " << fieldTypeToString(col.getType()) << endl;
    }
    return PrepareResult::PREPARE_SUCCESS;
}

PrepareResult prepare_insert_statement (string *statement) {

}

PrepareResult prepare_select_statement (string *statement) {

}

PrepareResult prepare_statement(string *statement, Schema &schema) {
    transform(statement->begin(), statement->end(), statement->begin(), [](unsigned char c){
        return tolower(c);
    });
    vector<string> tokens = split_on(statement, ' ');
    if (tokens[0] != "silly") {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }
    if (tokens[1] == "exit") {
        close_buffer(statement);
        cout << "Miss me please :(" << endl;
        return PrepareResult::PREPARE_SUCCESS;
    } else if (tokens[1] == "make") {
        return prepare_create_statement(statement, schema);
    } else if (tokens[1] == "put") {
        return prepare_insert_statement(statement);
    } else if (tokens[1] == "get") {
        return prepare_select_statement(statement);
    } else {
        return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
    }
}