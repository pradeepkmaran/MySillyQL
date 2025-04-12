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