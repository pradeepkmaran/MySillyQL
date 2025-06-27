#pragma once
#include "../common/types.hpp"
#include "schema.hpp"
#include "row.hpp"
#include "value.hpp"

class Table {
private:
    Schema schema;
    vector<Row> rows; 
    string filePath;
    bool isDirty; 
    
public:
    Table() = default; 
    Table(const Schema& schema, const string& filePath = "");
    
    Row createRow();
    void insertRow(const Row& row);
    vector<Row> getRows();
    vector<Row> findRows(const string& columnName, const Value value) const;
    bool updateRow(size_t index, const Row& updatedRow);
    bool deleteRow(size_t index);
    const Schema& getSchema() const;
    size_t size() const;
    bool saveToFile();
    bool loadFromFile();
};