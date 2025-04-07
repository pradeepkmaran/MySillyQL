// #include<bits/stdc++.h>
// #include"constants.hpp"
// using namespace std;

class ColumnDef {
private:
    string name;
    FieldType type;
    uint32_t maxSize;  // For variable length types like TEXT
    bool isPrimaryKey;

public:
    // ColumnDef(const string& name, FieldType type, uint32_t maxSize = 0, bool isPrimaryKey = false) 
    //     : name(name), type(type), maxSize(maxSize), isPrimaryKey(isPrimaryKey) {}
    ColumnDef() = default;
    ColumnDef(const string& name, FieldType type, bool isPrimaryKey = false) 
        : name(name), type(type), maxSize(0), isPrimaryKey(isPrimaryKey) {}

    const string& getName() const { return name; }
    FieldType getType() const { return type; }
    uint32_t getMaxSize() const { return maxSize; }
    bool getIsPrimaryKey() const { return isPrimaryKey; }
    
    uint32_t getSize() const {
        switch (type) {
            case FieldType::INTEGER: return sizeof(int64_t);
            case FieldType::DECIMAL: return sizeof(double);
            case FieldType::BOOLEAN: return sizeof(bool);
            case FieldType::TEXT: return maxSize;
            case FieldType::BLOB: return maxSize;
            default: return 0;
        }
    }
};

class Schema {
private:
    vector<ColumnDef> columns;
    string tableName;
    
public:
    Schema() : tableName("") {};
    Schema(const string& tableName) : tableName(tableName) {}
    
    void addColumn(const ColumnDef& column) {
        columns.push_back(column);
    }
    
    const vector<ColumnDef>& getColumns() const { return columns; }
    const string& getTableName() const { return tableName; }
    
    uint32_t getRowSize() const {
        uint32_t size = 0;
        for (const auto& col : columns) {
            size += col.getSize();
        }
        return size;
    }
    
    uint32_t getColumnOffset(const string& columnName) const {
        uint32_t offset = 0;
        for (const auto& col : columns) {
            if (col.getName() == columnName) {
                return offset;
            }
            offset += col.getSize();
        }
        return UINT32_MAX; // Column not found
    }
};

class Value {
private:
    FieldType type;
    union {
        int64_t intValue;
        double floatValue;
        bool boolValue;
        char* textValue;
    } data;
    uint32_t size;  // For TEXT and BLOB types

public:
    // Constructors for different types
    explicit Value(int64_t val) : type(FieldType::INTEGER), size(0) { data.intValue = val; }
    explicit Value(double val) : type(FieldType::DECIMAL), size(0) { data.floatValue = val; }
    explicit Value(bool val) : type(FieldType::BOOLEAN), size(0) { data.boolValue = val; }
    
    // Default constructor
    Value() : type(FieldType::TEXT), size(0) {
        data.textValue = nullptr;
    }

    // String constructor
    Value(const string& val) : type(FieldType::TEXT), size(val.length()) {
        data.textValue = new char[size + 1];
        strcpy(data.textValue, val.c_str());
    }
    
    // Destructor
    ~Value() {
        if ((type == FieldType::TEXT || type == FieldType::BLOB) && data.textValue != nullptr) {
            delete[] data.textValue;
        }
    }
    
    // Copy constructor
    Value(const Value& other) : type(other.type), size(other.size) {
        switch (type) {
            case FieldType::INTEGER:
                data.intValue = other.data.intValue;
                break;
            case FieldType::DECIMAL:
                data.floatValue = other.data.floatValue;
                break;
            case FieldType::BOOLEAN:
                data.boolValue = other.data.boolValue;
                break;
            case FieldType::TEXT:
            case FieldType::BLOB:
                if (other.data.textValue != nullptr) {
                    data.textValue = new char[size + 1];
                    strcpy(data.textValue, other.data.textValue);
                } else {
                    data.textValue = nullptr;
                }
                break;
        }
    }
    
    // Assignment operator
    Value& operator=(const Value& other) {
        if (this == &other) return *this;
        
        // Clean up existing resources
        if ((type == FieldType::TEXT || type == FieldType::BLOB) && data.textValue != nullptr) {
            delete[] data.textValue;
            data.textValue = nullptr;
        }
        
        type = other.type;
        size = other.size;
        
        switch (type) {
            case FieldType::INTEGER:
                data.intValue = other.data.intValue;
                break;
            case FieldType::DECIMAL:
                data.floatValue = other.data.floatValue;
                break;
            case FieldType::BOOLEAN:
                data.boolValue = other.data.boolValue;
                break;
            case FieldType::TEXT:
            case FieldType::BLOB:
                if (other.data.textValue != nullptr) {
                    data.textValue = new char[size + 1];
                    strcpy(data.textValue, other.data.textValue);
                } else {
                    data.textValue = nullptr;
                }
                break;
        }
        return *this;
    }
    
    // Debug function to print value type
    void printType() const {
        cout << "Value type: ";
        switch (type) {
            case FieldType::INTEGER: cout << "INTEGER"; break;
            case FieldType::DECIMAL: cout << "DECIMAL"; break;
            case FieldType::BOOLEAN: cout << "BOOLEAN"; break;
            case FieldType::TEXT: cout << "TEXT"; break;
            case FieldType::BLOB: cout << "BLOB"; break;
        }
        cout << endl;
    }
    
    // Getters
    FieldType getType() const { return type; }
    
    int64_t getInt() const { 
        if (type != FieldType::INTEGER) throw runtime_error("Type mismatch");
        return data.intValue; 
    }
    
    double getFloat() const {
        if (type != FieldType::DECIMAL) throw runtime_error("Type mismatch");
        return data.floatValue;
    }
    
    bool getBool() const {
        if (type != FieldType::BOOLEAN) throw runtime_error("Type mismatch");
        return data.boolValue;
    }
    
    string getText() const {
        if (type != FieldType::TEXT) {
            printType();  // Debug info
            throw runtime_error("Type mismatch");
        }
        return data.textValue ? string(data.textValue) : string("");
    }
    
    // Serialization methods
    void serialize(void* destination) const {
        char* dest = static_cast<char*>(destination);
        
        switch (type) {
            case FieldType::INTEGER:
                memcpy(dest, &data.intValue, sizeof(int64_t));
                break;
            case FieldType::DECIMAL:
                memcpy(dest, &data.floatValue, sizeof(double));
                break;
            case FieldType::BOOLEAN:
                memcpy(dest, &data.boolValue, sizeof(bool));
                break;
            case FieldType::TEXT:
            case FieldType::BLOB:
                if (data.textValue != nullptr) {
                    strncpy(dest, data.textValue, size);
                    dest[size] = '\0';
                } else {
                    dest[0] = '\0';
                }
                break;
        }
    }
    
    static Value deserialize(const void* source, FieldType type, uint32_t size) {
        const char* src = static_cast<const char*>(source);
        
        switch (type) {
            case FieldType::INTEGER: {
                int64_t value;
                memcpy(&value, src, sizeof(int64_t));
                return Value(value);
            }
            case FieldType::DECIMAL: {
                double value;
                memcpy(&value, src, sizeof(double));
                return Value(value);
            }
            case FieldType::BOOLEAN: {
                bool value;
                memcpy(&value, src, sizeof(bool));
                return Value(value);
            }
            case FieldType::TEXT:
            case FieldType::BLOB: {
                string value(src, strnlen(src, size));
                return Value(value);
            }
            default:
                return Value();
        }
    }
};

class Row {
private:
    map<string, Value> values;
    const Schema* schema;  // Reference to the schema this row belongs to

public:
    explicit Row(const Schema* schema) : schema(schema) {}
    
    void setValue(const string& columnName, const Value& value) {
        vector<ColumnDef> columns = schema->getColumns();
        if(find_if(columns.begin(), columns.end(), 
        [&columnName](const ColumnDef &col) {
            return col.getName() == columnName;
        }) == columns.end()) {
            throw runtime_error("Column not found: " + columnName);
        }
        values[columnName] = value;
    }
    
    Value getValue(const string& columnName) const {
        auto it = values.find(columnName);
        if (it == values.end()) {
            throw runtime_error("Column not found: " + columnName);
        }
        return it->second;
    }
    
    void serialize(void* destination) const {
        char* dest = static_cast<char*>(destination);
        
        for (const auto& col : schema->getColumns()) {
            const string& colName = col.getName();
            uint32_t offset = schema->getColumnOffset(colName);
            
            auto it = values.find(colName);
            if (it != values.end()) {
                it->second.serialize(dest + offset);
            } else {
                // Handle missing values
                memset(dest + offset, 0, col.getSize());
            }
        }
    }
    
    void deserialize(const void* source) {
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
};

class Table {
private:
    Schema schema;
    vector<Row> rows;  // Using simple string maps for storage
    string filePath;
    bool isDirty;  // Flag to track if table has unsaved changes
    
public:
    Table() = default; 
    Table(const Schema& schema, const string& filePath = "") 
        : schema(schema), filePath(filePath), isDirty(false) {}
    
    // Create a new row and return a reference to it
    Row createRow() {
        return Row(&schema);
    }
    
    // Insert a row into the table
    void insertRow(const Row& row) {
        rows.push_back(row);
        isDirty = true;
    }

    vector<Row> getRows() {
        return rows;
    }
    
    // Find rows by a column value
    vector<Row> findRows(const string& columnName, const Value value) const {
        vector<Row> result;
        ColumnDef target_column;
        vector<ColumnDef> columns = schema.getColumns();
        for(ColumnDef column: columns) {
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

    // Update a row at a specific index
    bool updateRow(size_t index, const Row& updatedRow) {
        if (index >= rows.size()) {
            return false;
        }
        
        rows[index] = updatedRow;
        isDirty = true;
        return true;
    }
    
    // Delete a row at a specific index
    bool deleteRow(size_t index) {
        if (index >= rows.size()) {
            return false;
        }
        
        rows.erase(rows.begin() + index);
        isDirty = true;
        return true;
    }
    
    // Get the schema
    const Schema& getSchema() const {
        return schema;
    }
    
    // Get number of rows
    size_t size() const {
        return rows.size();
    }
};

// Simple Database class to manage multiple tables
class Database {
private:
    map<string, Table> tables;
    string dbPath;
    
public:
    Database(const string& path) : dbPath(path) {
        // In a real implementation, you'd ensure the directory exists
    }
    
    // Create a new table
    void createTable(const Schema& schema) {
        string tableName = schema.getTableName();
        if (tableName.empty()) {
            throw runtime_error("Table must have a name");
        }
        
        if (tables.find(tableName) != tables.end()) {
            throw runtime_error("Table already exists: " + tableName);
        }
        
        string filePath = dbPath + "/" + tableName + ".db";
        tables.emplace(tableName, Table(schema, filePath));
    }
    
    // Get table by name
    Table& getTable(const string& tableName) {
        auto it = tables.find(tableName);
        if (it == tables.end()) {
            throw runtime_error("Table not found: " + tableName);
        }
        return it->second;
    }
    
    // Drop a table
    bool dropTable(const string& tableName) {
        auto it = tables.find(tableName);
        if (it == tables.end()) {
            return false;
        }
        
        tables.erase(it);
        return true;
    }
    
    // // Save all tables
    // void saveAll() {
    //     for (auto& pair : tables) {
    //         pair.second.saveToFile();
    //     }
    // }
    
    // // Load table from file
    // void loadTable(const string& tableName, const Schema& schema) {
    //     string filePath = dbPath + "/" + tableName + ".csv";
    //     tables[tableName] = Table::loadFromFile(filePath, schema);
    // }
};

// Example usage
// void exampleUsage() {
//     // Create schema for a "users" table
//     Schema userSchema("users");
//     userSchema.addColumn(ColumnDef("id", FieldType::INTEGER, true));
//     userSchema.addColumn(ColumnDef("name", FieldType::TEXT));
//     userSchema.addColumn(ColumnDef("email", FieldType::TEXT));
//     userSchema.addColumn(ColumnDef("age", FieldType::INTEGER));
//     userSchema.addColumn(ColumnDef("is_active", FieldType::BOOLEAN));
    
//     // Create database
//     Database db("./testdb");
    
//     // Create table
//     db.createTable(userSchema);
    
//     // Get the table
//     Table& usersTable = db.getTable("users");
    
//     // Insert data
//     Row user1 = usersTable.createRow();
//     user1.setValue("id", Value(int64_t(1)));
//     user1.setValue("name", Value("John Doe"));
//     user1.setValue("email", Value("john@example.com"));
//     user1.setValue("age", Value(int64_t(30)));
//     user1.setValue("is_active", Value(true));
//     usersTable.insertRow(user1);
    
//     Row user2 = usersTable.createRow();
//     user2.setValue("id", Value(int64_t(2)));
//     user2.setValue("name", Value("Jane Smith"));
//     user2.setValue("email", Value("jane@example.com"));
//     user2.setValue("age", Value(int64_t(25)));
//     user2.setValue("is_active", Value(true));
//     usersTable.insertRow(user2);
    
//     // Retrieve data by id
//     vector<Row> foundUsers = usersTable.findRows("id", int64_t(1));
//     if (!foundUsers.empty()) {
//         Row& foundUser = foundUsers[0];
//         cout << "Found user: " << foundUser.getValue("name").getText() << endl;
//     }
    
//     // Save data
//     db.saveAll();
// }