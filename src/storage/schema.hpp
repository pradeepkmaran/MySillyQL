#pragma once
#include "../common/types.hpp"

class ColumnDefinition {
private:
    string name;
    FieldType type;
    uint32_t maxSize;
    bool isPrimaryKey;

public:
    ColumnDefinition() = default;
    ColumnDefinition(const string& name, FieldType type, bool isPrimaryKey = false);

    const string& getName() const;
    FieldType getType() const;
    uint32_t getMaxSize() const;
    bool getIsPrimaryKey() const;
    uint32_t getSize() const;
};

class Schema {
private:
    vector<ColumnDefinition> columns;
    string tableName;
    
public:
    Schema();
    Schema(const string& tableName);
    
    void addColumn(const ColumnDefinition& column);
    const vector<ColumnDefinition>& getColumns() const;
    const string& getTableName() const;
    uint32_t getRowSize() const;
    uint32_t getColumnOffset(const string& columnName) const;
};