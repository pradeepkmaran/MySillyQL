
// silly make table <table_name> with columns <column1> of <integer> [as primary], <column2> of <text>, <column3> of <decimal>, <column4> of <boolean>;
PrepareResult prepare_create_statement (string *statement, Database &db) {
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

    Schema schema(tokens[3]);

    for (int token_index=6; token_index < (int32_t)(tokens.size()); ) {
        string colName = tokens[token_index++];
        
        if (tokens[token_index++] != "of") return PrepareResult::PREPARE_SYNTAX_ERROR;

        FieldType fieldType = stringToFieldType(tokens[token_index++]);
        bool isPrimary = false;
        if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "as" && token_index++) {
            if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "primary" && token_index++) {
                isPrimary = true;
            } else if (token_index >= (int32_t)(tokens.size())) {
                return PrepareResult::PREPARE_SYNTAX_ERROR;
            }
        }
        schema.addColumn(ColumnDef(colName, fieldType, isPrimary));
    }

    for (ColumnDef col: schema.getColumns()) {
        cout << col.getName() << " " << fieldTypeToString(col.getType()) << endl;
    }
    db.createTable(schema);
    return PrepareResult::PREPARE_SUCCESS;
}

// silly put <value1>, <value2>, <value3> inside table <table_name>;
PrepareResult prepare_insert_statement (string *statement, Database &db) {
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
    if (tokens[0] != "silly" || tokens[1] != "put") {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }
    vector<string> rawValues;
    string tableName = "";
    for(int token_index=2; token_index<tokens.size(); token_index++) {
        if(tokens[token_index] == "inside") {
            if(token_index+1<tokens.size() && tokens[token_index+1] == "table") {
                if(token_index+3<tokens.size()) {
                    return PrepareResult::PREPARE_SYNTAX_ERROR;
                } else if(token_index+2<tokens.size()){
                    tableName = tokens[token_index+2];
                    break;
                } else {
                    return PrepareResult::PREPARE_SYNTAX_ERROR;
                }
            } else {
                return PrepareResult::PREPARE_SYNTAX_ERROR;
            }
        } else {
            rawValues.push_back(tokens[token_index]);
        }
    }

    Table &table = db.getTable(tableName);
    Schema schema = table.getSchema();
    Row row = table.createRow();

    vector<ColumnDef> columns = schema.getColumns(); 
    if(columns.size() != rawValues.size()) {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }
    for(int index=0; index<columns.size(); index++) {
        ColumnDef column = columns[index];
        Value entry;
        if(isStringValidValueForFieldType(rawValues[index], column.getType())) {
            entry = stringToValue(rawValues[index], column.getType());
        } else {
            return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
        }
        row.setValue(column.getName(), entry);
    }
    table.insertRow(row);
    return PrepareResult::PREPARE_SUCCESS;
}

// silly make table mytable with columns reg of integer as primary, name of text, section of text;
// silly put 123 pradeep cseb inside table mytable;

PrepareResult prepare_select_statement (string *statement, Database &db) {
    Table table = db.getTable("mytable");
    vector<Row> rows = table.getRows();
    cout << rows.size() << endl;
    for(Row row: rows) {
        cout << row.getValue("reg").getInt() << " " << row.getValue("name").getText() << " " << row.getValue("section").getText() << endl;
    }
    return PrepareResult::PREPARE_SUCCESS;
}

PrepareResult prepare_statement(string *statement, Database &db) {
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
        return prepare_create_statement(statement, db);
    } else if (tokens[1] == "put") {
        return prepare_insert_statement(statement, db);
    } else if (tokens[1] == "get") {
        return prepare_select_statement(statement, db);
    } else {
        return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
    }
}