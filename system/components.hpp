// #include<bits/stdc++.h>
// #include"constants.hpp"
// using namespace std;

class ColumnDef {
private:
    string name;
    FieldType type;
    uint32_t maxSize;
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
        return UINT32_MAX; 
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
    uint32_t size;

public:
    explicit Value(int64_t val) : type(FieldType::INTEGER), size(0) { data.intValue = val; }
    explicit Value(double val) : type(FieldType::DECIMAL), size(0) { data.floatValue = val; }
    explicit Value(bool val) : type(FieldType::BOOLEAN), size(0) { data.boolValue = val; }

    Value() : type(FieldType::TEXT), size(0) {
        data.textValue = nullptr;
    }

    Value(const string& val) : type(FieldType::TEXT), size(val.length()) {
        data.textValue = new char[size + 1];
        strcpy(data.textValue, val.c_str());
    }
    
    ~Value() {
        if ((type == FieldType::TEXT || type == FieldType::BLOB) && data.textValue != nullptr) {
            delete[] data.textValue;
        }
    }
    
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
    
    Value& operator=(const Value& other) {
        if (this == &other) return *this;
        
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
    const Schema* schema;

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
    vector<Row> rows; 
    string filePath;
    bool isDirty; 
    
public:
    Table() = default; 
    Table(const Schema& schema, const string& filePath = "") 
        : schema(schema), filePath(filePath), isDirty(false) {}
    
    Row createRow() {
        return Row(&schema);
    }
    
    void insertRow(const Row& row) {
        rows.push_back(row);
        isDirty = true;
    }

    vector<Row> getRows() {
        return rows;
    }
    
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

    bool updateRow(size_t index, const Row& updatedRow) {
        if (index >= rows.size()) {
            return false;
        }
        
        rows[index] = updatedRow;
        isDirty = true;
        return true;
    }
    
    bool deleteRow(size_t index) {
        if (index >= rows.size()) {
            return false;
        }
        
        rows.erase(rows.begin() + index);
        isDirty = true;
        return true;
    }
    
    const Schema& getSchema() const {
        return schema;
    }
    
    size_t size() const { // Get number of rows
        return rows.size();
    }
    
    bool saveToFile() {
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
                            file.write(reinterpret_cast<char*>(&strLength), sizeof(uint32_t)); // store length of string
                            file.write(textVal.c_str(), strLength); // store the string
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

    bool loadFromFile() {
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
                        // Read string length first
                        uint32_t strLength;
                        file.read(reinterpret_cast<char*>(&strLength), sizeof(uint32_t));
                        
                        // Read string data
                        if (strLength > 0) {
                            vector<char> buffer(strLength + 1, 0); // +1 for null terminator
                            file.read(buffer.data(), strLength);
                            buffer[strLength] = '\0'; // Ensure null termination
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
};

class Database {
private:
    map<string, Table> tables;
    string dbName;
    string dbPath;
    
public:
    Database(const string& name) : dbName(name), dbPath("data/"+name) {}
    
    string getDbName() {
        return dbName;
    }
    
    void createTable(const Schema& schema) {
        string tableName = schema.getTableName();
        if (tableName.empty()) {
            throw runtime_error("Table must have a name");
        }
        
        if (tables.find(tableName) != tables.end()) {
            throw runtime_error("Table already exists: " + tableName);
        }
        
        filesystem::create_directories(dbPath);
        
        string schemaFilePath = dbPath + "/" + tableName + ".schema";
        if (!saveSchemaToFile(schema, schemaFilePath)) {
            throw runtime_error("Failed to save schema for table: " + tableName);
        }
        
        string dataFilePath = dbPath + "/" + tableName + ".db";
        tables.emplace(tableName, Table(schema, dataFilePath));
    }
    
    Table& getTable(const string& tableName) {
        auto it = tables.find(tableName);
        if (it == tables.end()) {
            throw runtime_error("Table not found: " + tableName);
        }
        return it->second;
    }
    
    bool dropTable(const string& tableName) {
        auto it = tables.find(tableName);
        if (it == tables.end()) {
            return false;
        }
        
        tables.erase(it);
        return true;
    }
    
    bool saveSchemaToFile(const Schema& schema, const string& filePath) {
        ofstream file(filePath, ios::binary | ios::out);
        if (!file) {
            return false;
        }
        string tableName = schema.getTableName();
        uint32_t nameLength = tableName.length();
        file.write(reinterpret_cast<char*>(&nameLength), sizeof(uint32_t));
        file.write(tableName.c_str(), nameLength);
        
        const vector<ColumnDef>& columns = schema.getColumns();
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

    bool saveAllTables() {
        bool allSaved = true;
        
        filesystem::create_directories(dbPath);
        
        for (auto& pair : tables) {
            const string& tableName = pair.first;
            Table& table = pair.second;
            
            string schemaFilePath = dbPath + "/" + tableName + ".schema";
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
    
    Schema loadSchemaFromFile(const string& filePath) {
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
            
            ColumnDef column(colName, type, isPrimaryKey);
            schema.addColumn(column);
        }
        
        file.close();
        return schema;
    }

    bool loadAllTables() {
        string dbDirectory = dbPath;
        vector<string> tableFiles;
        
        filesystem::create_directories(dbDirectory);
        
        for (const auto& entry : filesystem::directory_iterator(dbDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".db") {
                tableFiles.push_back(entry.path().filename().string());
            }
        }
        
        bool allLoaded = true;
        
        for (const auto& tableFile : tableFiles) {
            // Extract table name from filename (remove .db extension)
            string tableName = tableFile.substr(0, tableFile.length() - 3);
            
            // First, load the schema from a separate schema file
            string schemaFilePath = dbPath + "/" + tableName + ".schema";
            Schema schema = loadSchemaFromFile(schemaFilePath);
            
            if (schema.getTableName().empty()) {
                cerr << "Failed to load schema for table: " << tableName << endl;
                allLoaded = false;
                continue;
            }
            
            string dataFilePath = dbPath + "/" + tableName + ".db";
            tables[tableName] = Table(schema, dataFilePath);
            
            if (!tables[tableName].loadFromFile()) {
                cerr << "Failed to load data for table: " << tableName << endl;
                allLoaded = false;
            }
        }
        return allLoaded;
    }
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