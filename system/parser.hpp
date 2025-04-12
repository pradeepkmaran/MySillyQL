vector<string> tokenize(string *statement) {
    vector<string> tokens = split_on(statement, ' ');
    vector<string> fine_tokens;
    
    for (int token_index = 0; token_index < (int32_t)(tokens.size()); token_index++) {
        vector<string> delimits1 = split_on(&tokens[token_index], ',');
        for (string delimit1 : delimits1) {
            vector<string> delimits2 = split_on(&delimit1, ';');
            for (string delimit2 : delimits2) {
                fine_tokens.push_back(delimit2);
            }
        }
    }

    return fine_tokens;
}

SillyResults prepare_create_db(string* statement, Database& db, string &dbName) {
    vector<string> tokens = tokenize(statement);
    
    if (tokens.size() != 4 || tokens[0] != "silly" || tokens[1] != "make" || tokens[2] != "db") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }
    
    dbName = tokens[3];
    return SillyResults::SILLY_SUCCESS;
}

SillyResults prepare_create_statement(string *statement, Schema &outSchema) {
    vector<string> tokens = tokenize(statement);

    if (tokens[0] != "silly" || tokens[1] != "make" || tokens[2] != "table" || tokens[4] != "with" || tokens[5] != "columns") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }

    outSchema = Schema(tokens[3]);

    for (int token_index=6; token_index < (int32_t)(tokens.size()); ) {
        string colName = tokens[token_index++];
        
        if (tokens[token_index++] != "of") return SillyResults::SILLY_SYNTAX_ERROR;

        FieldType fieldType = stringToFieldType(tokens[token_index++]);
        bool isPrimary = false;
        if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "as" && token_index++) {
            if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "primary" && token_index++) {
                isPrimary = true;
            } else if (token_index >= (int32_t)(tokens.size())) {
                return SillyResults::SILLY_SYNTAX_ERROR;
            }
        }
        outSchema.addColumn(ColumnDef(colName, fieldType, isPrimary));
    }

    return SillyResults::SILLY_SUCCESS;
}

SillyResults prepare_insert_statement(string *statement, string &outTableName, vector<string> &outRawValues) {
    vector<string> tokens = tokenize(statement);
    if (tokens[0] != "silly" || tokens[1] != "put") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }
    
    outRawValues.clear();
    outTableName = "";
    
    for(int token_index=2; token_index<tokens.size(); token_index++) {
        if(tokens[token_index] == "inside") {
            if(token_index+1<tokens.size() && tokens[token_index+1] == "table") {
                if(token_index+3<tokens.size()) {
                    return SillyResults::SILLY_SYNTAX_ERROR;
                } else if(token_index+2<tokens.size()){
                    outTableName = tokens[token_index+2];
                    break;
                } else {
                    return SillyResults::SILLY_SYNTAX_ERROR;
                }
            } else {
                return SillyResults::SILLY_SYNTAX_ERROR;
            }
        } else {
            outRawValues.push_back(tokens[token_index]);
        }
    }

    return SillyResults::SILLY_SUCCESS;
}

SillyResults prepare_select_statement(string *statement, string &outTableName) {
    vector<string> tokens = tokenize(statement);

    return SillyResults::SILLY_SUCCESS;
}

SillyResults prepare_statement(string *statement, Database &db) {
    if (statement->back() != ';') {
        return SillyResults::SILLY_SEMICOLON_MISSING_ERROR;
    }
    vector<string> tokens = tokenize(statement);
    if (tokens[0] != "silly") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }

    if (tokens[1] == "exit") { 
        close_buffer(statement);
        cout << "Miss me please :(" << endl;
        exit(EXIT_SUCCESS);
        return SillyResults::SILLY_SUCCESS;
    } else if (tokens[1] == "make") {
        if(tokens[2] == "table") {
            Schema schema;
            SillyResults parseResult = prepare_create_statement(statement, schema);
            if (parseResult != SillyResults::SILLY_SUCCESS) {
                return parseResult;
            }
            return execute_create_table(schema, db);
        } else if (tokens[2] == "db") {
            string dbName;
            SillyResults parseResult = prepare_create_db(statement, db, dbName);
            if (parseResult != SillyResults::SILLY_SUCCESS) {
                return parseResult;
            }
            return execute_create_db(db, dbName);
        } else {
            return SillyResults::SILLY_SYNTAX_ERROR;
        }
    } else if (tokens[1] == "put") {
        string tableName;
        vector<string> rawValues;
        SillyResults parseResult = prepare_insert_statement(statement, tableName, rawValues);
        if (parseResult != SillyResults::SILLY_SUCCESS) {
            return parseResult;
        }
        return execute_insert(tableName, rawValues, db);
    } else if (tokens[1] == "get") {
        string tableName = tokens[2];
        SillyResults parseResult = prepare_select_statement(statement, tableName);
        if (parseResult != SillyResults::SILLY_SUCCESS) {
            return parseResult;
        }
        return execute_select(tableName, db);
    } else if (tokens[1] == "save") {
        db.saveAllTables();
        return SillyResults::SILLY_SUCCESS;
    } else if (tokens[1] == "use") {
        db = Database(tokens[2]);
        db.loadAllTables();
        return SillyResults::SILLY_SUCCESS;
    } else {
        return SillyResults::SILLY_UNRECOGNIZED_STATEMENT;
    }
}