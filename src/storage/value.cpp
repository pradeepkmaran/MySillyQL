#include "value.hpp"

Value::Value(int64_t val) : type(FieldType::INTEGER), size(0) { 
    data.intValue = val; 
}

Value::Value(double val) : type(FieldType::DECIMAL), size(0) { 
    data.floatValue = val; 
}

Value::Value(bool val) : type(FieldType::BOOLEAN), size(0) { 
    data.boolValue = val; 
}

Value::Value() : type(FieldType::TEXT), size(0) {
    data.textValue = nullptr;
}

Value::Value(const string& val) : type(FieldType::TEXT), size(val.length()) {
    data.textValue = new char[size + 1];
    strcpy(data.textValue, val.c_str());
}

Value::~Value() {
    if ((type == FieldType::TEXT || type == FieldType::BLOB) && data.textValue != nullptr) {
        delete[] data.textValue;
    }
}

Value::Value(const Value& other) : type(other.type), size(other.size) {
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

Value& Value::operator=(const Value& other) {
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

void Value::printType() const {
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

FieldType Value::getType() const { 
    return type; 
}

int64_t Value::getInt() const { 
    if (type != FieldType::INTEGER) throw runtime_error("Type mismatch");
    return data.intValue; 
}

double Value::getFloat() const {
    if (type != FieldType::DECIMAL) throw runtime_error("Type mismatch");
    return data.floatValue;
}

bool Value::getBool() const {
    if (type != FieldType::BOOLEAN) throw runtime_error("Type mismatch");
    return data.boolValue;
}

string Value::getText() const {
    if (type != FieldType::TEXT) {
        printType();
        throw runtime_error("Type mismatch");
    }
    return data.textValue ? string(data.textValue) : string("");
}

void Value::serialize(void* destination) const {
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

Value Value::deserialize(const void* source, FieldType type, uint32_t size) {
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