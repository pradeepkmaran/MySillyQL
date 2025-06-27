#include "database.hpp"

Database::Database(const string& name) : name(name), path("data/"+name) {}

string Database::getName() const {
    return name;
}

void Database::createTable(const Schema& schema) {
    string tableName = schema.getTableName();
    if (tableName.empty()) {
        throw runtime_error("Table must have a name");
    }
    
    if (tables.find(tableName) != tables.end()) {
        throw runtime_error("Table already exists: " + tableName);
    }
    
    filesystem::create_directories(path);
    
    string schemaFilePath = path + "/" + tableName + ".schema";
    if (!saveSchemaToFile(schema, schemaFilePath)) {
        throw runtime_error("Failed to save schema for table: " + tableName);
    }
    
    string dataFilePath = path + "/" + tableName + ".db";
    tables.emplace(tableName, Table(schema, dataFilePath));
}

Table& Database::getTable(const string& tableName) {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
        throw runtime_error("Table not found: " + tableName);
    }
    return it->second;
}

bool Database::dropTable(const string& tableName) {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
        return false;
    }
    
    tables.erase(it);
    return true;
}

bool Database::saveSchemaToFile(const Schema& schema, const string& filePath) {
    ofstream file(filePath, ios::binary | ios::out);
    if (!file) {
        return false;
    }
    string tableName = schema.getTableName();
    uint32_t nameLength = tableName.length();
    file.write(reinterpret_cast<char*>(&nameLength), sizeof(uint32_t));
    file.write(tableName.c_str(), nameLength);
    
    const vector<ColumnDefinition>& columns = schema.getColumns();
    uint32_t numColumns = columns.size();
    file.write(reinterpret_cast<char*>(&numColumns), sizeof(uint32_t));
    
    for (const auto& col : columns) {
        string colName = col.getName();
        uint32_t colNameLength = colName.length();
        file.write(reinterpret_cast<char*>(&colNameLength), sizeof(uint32_t));
        file.write(colName.c_str(), colNameLength);
        
        int typeInt = static_cast<int>(col.getType());
        file.write(reinterpret_cast<char*>(&typeInt), sizeof(int));
        
        uint32_t maxSize = col.getMaxSize();
        file.write(reinterpret_cast<char*>(&maxSize), sizeof(uint32_t));
        
        bool isPrimaryKey = col.getIsPrimaryKey();
        file.write(reinterpret_cast<char*>(&isPrimaryKey), sizeof(bool));
    }
    
    file.close();
    return true;
}

bool Database::saveAllTables() {
    bool allSaved = true;
    
    filesystem::create_directories(path);
    
    for (auto& pair : tables) {
        const string& tableName = pair.first;
        Table& table = pair.second;
        
        string schemaFilePath = path + "/" + tableName + ".schema";
        if (!saveSchemaToFile(table.getSchema(), schemaFilePath)) {
            cerr << "Failed to save schema for table: " << tableName << endl;
            allSaved = false;
        }
        
        if (!table.saveToFile()) {
            cerr << "Failed to save data for table: " << tableName << endl;
            allSaved = false;
        }
    }
    return allSaved;
}

Schema Database::loadSchemaFromFile(const string& filePath) {
    ifstream file(filePath, ios::binary | ios::in);
    if (!file) {
        return Schema();
    }
    
    uint32_t nameLength;
    file.read(reinterpret_cast<char*>(&nameLength), sizeof(uint32_t));
    
    vector<char> nameBuffer(nameLength + 1, 0);
    file.read(nameBuffer.data(), nameLength);
    nameBuffer[nameLength] = '\0';
    
    string tableName(nameBuffer.data());
    Schema schema(tableName);
    
    uint32_t numColumns;
    file.read(reinterpret_cast<char*>(&numColumns), sizeof(uint32_t));
    
    for (uint32_t i = 0; i < numColumns; i++) {
        uint32_t colNameLength;
        file.read(reinterpret_cast<char*>(&colNameLength), sizeof(uint32_t));
        
        vector<char> colNameBuffer(colNameLength + 1, 0);
        file.read(colNameBuffer.data(), colNameLength);
        colNameBuffer[colNameLength] = '\0';
        string colName(colNameBuffer.data());
        
        int typeInt;
        file.read(reinterpret_cast<char*>(&typeInt), sizeof(int));
        FieldType type = static_cast<FieldType>(typeInt);
        
        uint32_t maxSize;
        file.read(reinterpret_cast<char*>(&maxSize), sizeof(uint32_t));
        
        bool isPrimaryKey;
        file.read(reinterpret_cast<char*>(&isPrimaryKey), sizeof(bool));
        
        ColumnDefinition column(colName, type, isPrimaryKey);
        schema.addColumn(column);
    }
    
    file.close();
    return schema;
}

bool Database::loadAllTables() {
    string dbDirectory = path;
    vector<string> tableFiles;
    
    filesystem::create_directories(dbDirectory);
    
    for (const auto& entry : filesystem::directory_iterator(dbDirectory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".db") {
            tableFiles.push_back(entry.path().filename().string());
        }
    }
    
    bool allLoaded = true;
    
    for (const auto& tableFile : tableFiles) {
        string tableName = tableFile.substr(0, tableFile.length() - 3);
        
        string schemaFilePath = path + "/" + tableName + ".schema";
        Schema schema = loadSchemaFromFile(schemaFilePath);
        
        if (schema.getTableName().empty()) {
            cerr << "Failed to load schema for table: " << tableName << endl;
            allLoaded = false;
            continue;
        }
        
        string dataFilePath = path + "/" + tableName + ".db";
        tables[tableName] = Table(schema, dataFilePath);
        
        if (!tables[tableName].loadFromFile()) {
            cerr << "Failed to load data for table: " << tableName << endl;
            allLoaded = false;
        }
    }
    return allLoaded;
}