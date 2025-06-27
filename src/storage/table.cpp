#include "table.hpp"

Table::Table(const Schema& schema, const string& filePath) 
    : schema(schema), filePath(filePath), isDirty(false) {}

Row Table::createRow() {
    return Row(&schema);
}

void Table::insertRow(const Row& row) {
    rows.push_back(row);
    isDirty = true;
}

vector<Row> Table::getRows() {
    return rows;
}

vector<Row> Table::findRows(const string& columnName, const Value value) const {
    vector<Row> result;
    ColumnDefinition target_column;
    vector<ColumnDefinition> columns = schema.getColumns();
    for(ColumnDefinition column: columns) {
        if(column.getName() == columnName) {
            target_column = column;
            break;
        }
    }
    FieldType type = target_column.getType();
    for(Row row: rows) {
        switch(type) {
            case FieldType::INTEGER:
                if(row.getValue(columnName).getInt() == value.getInt()) result.push_back(row);
                break;
            case FieldType::DECIMAL:
                if(row.getValue(columnName).getFloat() == value.getFloat()) result.push_back(row);
                break;
            case FieldType::BOOLEAN:
                if(row.getValue(columnName).getBool() == value.getBool()) result.push_back(row);
                break;
            case FieldType::TEXT:
            case FieldType::BLOB:
                if(row.getValue(columnName).getText() == value.getText()) result.push_back(row);
                break;
        }
    }
    return result;
}

bool Table::updateRow(size_t index, const Row& updatedRow) {
    if (index >= rows.size()) {
        return false;
    }
    
    rows[index] = updatedRow;
    isDirty = true;
    return true;
}

bool Table::deleteRow(size_t index) {
    if (index >= rows.size()) {
        return false;
    }
    
    rows.erase(rows.begin() + index);
    isDirty = true;
    return true;
}

const Schema& Table::getSchema() const {
    return schema;
}

size_t Table::size() const {
    return rows.size();
}

bool Table::saveToFile() {
    if (filePath.empty()) {
        return false;
    }

    ofstream file(filePath, ios::binary | ios::out);
    if (!file) {
        return false;
    }

    size_t numRows = rows.size();
    file.write(reinterpret_cast<char*>(&numRows), sizeof(size_t));

    for (const Row& row : rows) {
        for (const auto& col : schema.getColumns()) {
            const string& colName = col.getName();
            FieldType type = col.getType();
            
            try {
                Value value = row.getValue(colName);
                
                switch (type) {
                    case FieldType::INTEGER: {
                        int64_t intVal = value.getInt();
                        file.write(reinterpret_cast<char*>(&intVal), sizeof(int64_t));
                        break;
                    }
                    case FieldType::DECIMAL: {
                        double floatVal = value.getFloat();
                        file.write(reinterpret_cast<char*>(&floatVal), sizeof(double));
                        break;
                    }
                    case FieldType::BOOLEAN: {
                        bool boolVal = value.getBool();
                        file.write(reinterpret_cast<char*>(&boolVal), sizeof(bool));
                        break;
                    }
                    case FieldType::TEXT:
                    case FieldType::BLOB: {
                        string textVal = value.getText();
                        uint32_t strLength = textVal.length();
                        file.write(reinterpret_cast<char*>(&strLength), sizeof(uint32_t));
                        file.write(textVal.c_str(), strLength);
                        break;
                    }
                }
            } catch (const runtime_error& e) {
                switch (type) {
                    case FieldType::INTEGER: {
                        int64_t defaultInt = 0;
                        file.write(reinterpret_cast<char*>(&defaultInt), sizeof(int64_t));
                        break;
                    }
                    case FieldType::DECIMAL: {
                        double defaultFloat = 0.0;
                        file.write(reinterpret_cast<char*>(&defaultFloat), sizeof(double));
                        break;
                    }
                    case FieldType::BOOLEAN: {
                        bool defaultBool = false;
                        file.write(reinterpret_cast<char*>(&defaultBool), sizeof(bool));
                        break;
                    }
                    case FieldType::TEXT:
                    case FieldType::BLOB: {
                        uint32_t strLength = 0;
                        file.write(reinterpret_cast<char*>(&strLength), sizeof(uint32_t));
                        break;
                    }
                }
            }
        }
    }
    
    file.close();
    isDirty = false;
    return true;
}

bool Table::loadFromFile() {
    if (filePath.empty()) {
        return false;
    }
    
    ifstream file(filePath, ios::binary | ios::in);
    if (!file) {
        return false;
    }
    
    rows.clear();

    size_t numRows;
    file.read(reinterpret_cast<char*>(&numRows), sizeof(size_t));
    
    for (size_t i = 0; i < numRows; i++) {
        Row newRow(&schema);
        
        for (const auto& col : schema.getColumns()) {
            const string& colName = col.getName();
            FieldType type = col.getType();
            
            switch (type) {
                case FieldType::INTEGER: {
                    int64_t intVal;
                    file.read(reinterpret_cast<char*>(&intVal), sizeof(int64_t));
                    newRow.setValue(colName, Value(intVal));
                    break;
                }
                case FieldType::DECIMAL: {
                    double floatVal;
                    file.read(reinterpret_cast<char*>(&floatVal), sizeof(double));
                    newRow.setValue(colName, Value(floatVal));
                    break;
                }
                case FieldType::BOOLEAN: {
                    bool boolVal;
                    file.read(reinterpret_cast<char*>(&boolVal), sizeof(bool));
                    newRow.setValue(colName, Value(boolVal));
                    break;
                }
                case FieldType::TEXT:
                case FieldType::BLOB: {
                    uint32_t strLength;
                    file.read(reinterpret_cast<char*>(&strLength), sizeof(uint32_t));
                    
                    if (strLength > 0) {
                        vector<char> buffer(strLength + 1, 0);
                        file.read(buffer.data(), strLength);
                        buffer[strLength] = '\0';
                        newRow.setValue(colName, Value(string(buffer.data())));
                    } else {
                        newRow.setValue(colName, Value(string("")));
                    }
                    break;
                }
            }
        }
        rows.push_back(newRow);
    }
    file.close();
    isDirty = false;
    return true;
}