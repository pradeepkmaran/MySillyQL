SillyResults execute_create_db(Database& db, string &dbName) {
    try {
        string dataDir = "data";
        filesystem::path dataPath(dataDir);
        
        if (!filesystem::exists(dataPath)) {
            if (!filesystem::create_directory(dataPath)) {
                cout << "Failed to create data directory" << endl;
                return SillyResults::SILLY_EXECUTION_ERROR;
            }
        }

        db = Database(dbName);
        
        string dbDir = dataDir + "/" + db.getDbName();
        filesystem::path dbPath(dbDir);
        
        if (filesystem::exists(dbPath)) {
            cout << "Database '" << db.getDbName() << "' already exists" << endl;
            return SillyResults::SILLY_EXECUTION_ERROR;
        }
        
        if (!filesystem::create_directory(dbPath)) {
            cout << "Failed to create database directory" << endl;
            return SillyResults::SILLY_EXECUTION_ERROR;
        }
        
        string configFile = dbDir + "/config.dbf";
        ofstream configStream(configFile);
        
        if (!configStream.is_open()) {
            cout << "Failed to create config file" << endl;
            return SillyResults::SILLY_EXECUTION_ERROR;
        }
        
        configStream << "DB_NAME=" << db.getDbName() << endl;
        configStream << "CREATED_AT=" << time(nullptr) << endl;
        configStream.close();
        
        cout << "Database '" << db.getDbName() << "' created successfully" << endl;
        return SillyResults::SILLY_SUCCESS;
    } 
    catch (const exception& e) {
        cout << "Error creating database: " << e.what() << endl;
        return SillyResults::SILLY_EXECUTION_ERROR;
    }
}

SillyResults execute_create_table(Schema &schema, Database &db) {
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
        const Schema& schema = table.getSchema();
        vector<Row> rows = table.getRows();
        
        const vector<ColumnDef>& columns = schema.getColumns();
        vector<string> columnNames;
        
        for (const auto& col : columns) {
            columnNames.push_back(col.getName());
        }
        
        cout << "GOT " << rows.size() << (rows.size() > 1 ? " ROWS" : " ROW") << endl;
        
        if (rows.empty()) {
            return SillyResults::SILLY_SUCCESS;
        }
        
        vector<size_t> columnWidths(columnNames.size(), 0);
        
        for (size_t i = 0; i < columnNames.size(); i++) {
            columnWidths[i] = columnNames[i].length();
        }
        
        for (const Row& row : rows) {
            for (size_t i = 0; i < columnNames.size(); i++) {
                const string& colName = columnNames[i];
                size_t valueLength = 0;
                
                try {
                    Value value = row.getValue(colName);
                    FieldType type = value.getType();
                    
                    switch (type) {
                        case FieldType::INTEGER: {
                            valueLength = to_string(value.getInt()).length();
                            break;
                        }
                        case FieldType::DECIMAL: {
                            valueLength = to_string(value.getFloat()).length();
                            break;
                        }
                        case FieldType::BOOLEAN: {
                            valueLength = value.getBool() ? 4 : 5;
                            break;
                        }
                        case FieldType::TEXT:
                        case FieldType::BLOB: {
                            valueLength = value.getText().length();
                            break;
                        }
                    }
                } catch (const exception& e) {
                    valueLength = 4; 
                }
                
                columnWidths[i] = max(columnWidths[i], valueLength);
            }
        }
        
        for (size_t i = 0; i < columnWidths.size(); i++) {
            columnWidths[i] += 2;
        }
        
        printTableBorder(columnWidths);
        
        cout << "|";
        for (size_t i = 0; i < columnNames.size(); i++) {
            cout << " " << setw(columnWidths[i] - 2) << left << columnNames[i] << " |";
        }
        cout << endl;
        
        printTableBorder(columnWidths);
        
        for (const Row& row : rows) {
            cout << "|";
            for (size_t i = 0; i < columnNames.size(); i++) {
                const string& colName = columnNames[i];
                
                try {
                    Value value = row.getValue(colName);
                    FieldType type = value.getType();
                    
                    cout << " ";
                    switch (type) {
                        case FieldType::INTEGER:
                            cout << setw(columnWidths[i] - 2) << left << value.getInt();
                            break;
                        case FieldType::DECIMAL:
                            cout << setw(columnWidths[i] - 2) << left << value.getFloat();
                            break;
                        case FieldType::BOOLEAN:
                            cout << setw(columnWidths[i] - 2) << left << (value.getBool() ? "true" : "false");
                            break;
                        case FieldType::TEXT:
                        case FieldType::BLOB:
                            cout << setw(columnWidths[i] - 2) << left << value.getText();
                            break;
                    }
                    cout << " ";
                } catch (const exception& e) {
                    cout << " " << setw(columnWidths[i] - 2) << left << "NULL" << " ";
                }
                cout << "|";
            }
            cout << endl;
        }
        
        printTableBorder(columnWidths);
        
        return SillyResults::SILLY_SUCCESS;
    } catch (const exception& e) {
        cout << "Error executing select: " << e.what() << endl;
        return SillyResults::SILLY_EXECUTION_ERROR;
    }
}
