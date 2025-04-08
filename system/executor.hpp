

SillyResults execute_create_table(Schema &schema, Database &db) {
    // Output column information (purely for display purposes)
    for (ColumnDef col: schema.getColumns()) {
        cout << col.getName() << " " << fieldTypeToString(col.getType()) << endl;
    }
    
    try {
        db.createTable(schema);
        return SillyResults::SILLY_SUCCESS;
    } catch (const exception& e) {
        cout << "Error creating table: " << e.what() << endl;
        return SillyResults::SILLY_EXECUTION_ERROR;
    }
}

SillyResults execute_insert(string tableName, vector<string> &rawValues, Database &db) {
    try {
        Table &table = db.getTable(tableName);
        Schema schema = table.getSchema();
        Row row = table.createRow();

        vector<ColumnDef> columns = schema.getColumns(); 
        if(columns.size() != rawValues.size()) {
            return SillyResults::SILLY_SYNTAX_ERROR;
        }
        
        for(int index=0; index<columns.size(); index++) {
            ColumnDef column = columns[index];
            Value entry;
            if(isStringValidValueForFieldType(rawValues[index], column.getType())) {
                entry = stringToValue(rawValues[index], column.getType());
            } else {
                return SillyResults::SILLY_TYPE_MISMATCH_ERROR;
            }
            row.setValue(column.getName(), entry);
        }
        
        table.insertRow(row);
        return SillyResults::SILLY_SUCCESS;
    } catch (const exception& e) {
        cout << "Error executing insert: " << e.what() << endl;
        return SillyResults::SILLY_EXECUTION_ERROR;
    }
}

SillyResults execute_select(string tableName, Database &db) {
    try {
        Table table = db.getTable(tableName);
        vector<Row> rows = table.getRows();
        
        cout << rows.size() << endl;
        for(Row row: rows) {
            cout << row.getValue("reg").getInt() << " " << row.getValue("name").getText() << endl;
        }
        
        return SillyResults::SILLY_SUCCESS;
    } catch (const exception& e) {
        cout << "Error executing select: " << e.what() << endl;
        return SillyResults::SILLY_EXECUTION_ERROR;
    }
}