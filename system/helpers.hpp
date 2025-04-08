// #include "imports.hpp"
void print_prompt () {
    cout << "silly % ";
}

void read_input (string *input_buffer) {
    getline(cin, *input_buffer);
}

void close_buffer (string *input_buffer) {
    delete input_buffer;
}

vector<string> split_on(string *str, char ch) {
    stringstream ss(*str);
    vector<string> words;
    string word;
    while(getline(ss, word, ch)) {
        words.push_back(word);
    }
    return words;
}

string fieldTypeToString(FieldType type) {
    switch (type) {
        case FieldType::INTEGER: return "INTEGER";
        case FieldType::TEXT: return "TEXT";
        case FieldType::DECIMAL: return "DECIMAL";
        case FieldType::BOOLEAN: return "BOOLEAN";
        case FieldType::BLOB: return "BOOLEAN";
        default: return "UNKNOWN";
    }
}

FieldType stringToFieldType(string typeStr) {
    if (typeStr == "integer") return FieldType::INTEGER;
    if (typeStr == "text") return FieldType::TEXT;
    if (typeStr == "decimal") return FieldType::DECIMAL;
    if (typeStr == "boolean") return FieldType::BOOLEAN;
    if (typeStr == "blob") return FieldType::BLOB;
    return FieldType::UNKNOWN;
}

bool isStringValidValueForFieldType(const string& value, FieldType type) {
    try {
        switch(type) {
            case FieldType::INTEGER: {
                stoi(value);
                return true;
            }
            case FieldType::DECIMAL: {
                stof(value);
                return true;
            }
            case FieldType::BOOLEAN: {
                return value == "true" || value == "false";
            }
            case FieldType::TEXT: {
                return true;
            }
            case FieldType::BLOB: {
                return true;
            }
            default:
                return false;
        }
    } catch(const exception&) {
        return false;
    }
}

Value stringToValue(const string& str, FieldType type) {
    Value result;
    
    switch (type) {
        case FieldType::INTEGER: {
            try {
                int num = stoi(str);
                result = Value((int64_t)num);
            } catch (const exception& e) {
                throw runtime_error("Failed to convert '" + str + "' to INTEGER: " + e.what());
            }
            break;
        }
        case FieldType::DECIMAL: {
            try {
                float num = stof(str);
                result = Value((float)num);
            } catch (const exception& e) {
                throw runtime_error("Failed to convert '" + str + "' to DECIMAL: " + e.what());
            }
            break;
        }
        case FieldType::BOOLEAN: {                          
            if (str == "true" || str == "1") {
                result = Value(true);
            } else if (str == "false" || str == "0") {
                result = Value(false);
            } else {
                throw runtime_error("Failed to convert '" + str + "' to BOOL. Expected 'true', 'false', '1', or '0'");
            }
            break;
        }
        case FieldType::TEXT: {
            result = Value(str);
            break;
        }
        default:
            throw runtime_error("Unsupported field type for conversion");
    }
    
    return result;
}