#include "row.hpp"

Row::Row(const Schema* schema) : schema(schema) {}

void Row::setValue(const string& columnName, const Value& value) {
    vector<ColumnDefinition> columns = schema->getColumns();
    if(find_if(columns.begin(), columns.end(), 
    [&columnName](const ColumnDefinition &col) {
        return col.getName() == columnName;
    }) == columns.end()) {
        throw runtime_error("Column not found: " + columnName);
    }
    values[columnName] = value;
}

Value Row::getValue(const string& columnName) const {
    auto it = values.find(columnName);
    if (it == values.end()) {
        throw runtime_error("Column not found: " + columnName);
    }
    return it->second;
}

void Row::serialize(void* destination) const {
    char* dest = static_cast<char*>(destination);
    
    for (const auto& col : schema->getColumns()) {
        const string& colName = col.getName();
        uint32_t offset = schema->getColumnOffset(colName);
        
        auto it = values.find(colName);
        if (it != values.end()) {
            it->second.serialize(dest + offset);
        } else {
            memset(dest + offset, 0, col.getSize());
        }
    }
}

void Row::deserialize(const void* source) {
    const char* src = static_cast<const char*>(source);
    
    for (const auto& col : schema->getColumns()) {
        const string& colName = col.getName();
        uint32_t offset = schema->getColumnOffset(colName);
        FieldType type = col.getType();
        uint32_t size = col.getSize();
        
        Value value = Value::deserialize(src + offset, type, size);
        values[colName] = value;
    }
}