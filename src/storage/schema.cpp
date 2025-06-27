#include "schema.hpp"

ColumnDefinition::ColumnDefinition(const string& name, FieldType type, bool isPrimaryKey) 
    : name(name), type(type), maxSize(0), isPrimaryKey(isPrimaryKey) {}

const string& ColumnDefinition::getName() const { 
    return name; 
}

FieldType ColumnDefinition::getType() const { 
    return type; 
}

uint32_t ColumnDefinition::getMaxSize() const { 
    return maxSize; 
}

bool ColumnDefinition::getIsPrimaryKey() const { 
    return isPrimaryKey; 
}

uint32_t ColumnDefinition::getSize() const {
    switch (type) {
        case FieldType::INTEGER: return sizeof(int64_t);
        case FieldType::DECIMAL: return sizeof(double);
        case FieldType::BOOLEAN: return sizeof(bool);
        case FieldType::TEXT: return maxSize;
        case FieldType::BLOB: return maxSize;
        default: return 0;
    }
}

Schema::Schema() : tableName("") {}

Schema::Schema(const string& tableName) : tableName(tableName) {}

void Schema::addColumn(const ColumnDefinition& column) {
    columns.push_back(column);
}

const vector<ColumnDefinition>& Schema::getColumns() const { 
    return columns; 
}

const string& Schema::getTableName() const { 
    return tableName; 
}

uint32_t Schema::getRowSize() const {
    uint32_t size = 0;
    for (const auto& col : columns) {
        size += col.getSize();
    }
    return size;
}

uint32_t Schema::getColumnOffset(const string& columnName) const {
    uint32_t offset = 0;
    for (const auto& col : columns) {
        if (col.getName() == columnName) {
            return offset;
        }
        offset += col.getSize();
    }
    return UINT32_MAX; 
}