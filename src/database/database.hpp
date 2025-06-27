#pragma once
#include "../common/types.hpp"
#include "../storage/table.hpp"
#include "../storage/schema.hpp"

class Database {
private:
    map<string, Table> tables;
    string name;
    string path;
    
public:
    Database(const string& name);
    
    string getName() const;
    void createTable(const Schema& schema);
    Table& getTable(const string& tableName);
    bool dropTable(const string& tableName);
    bool saveSchemaToFile(const Schema& schema, const string& filePath);
    bool saveAllTables();
    Schema loadSchemaFromFile(const string& filePath);
    bool loadAllTables();
};