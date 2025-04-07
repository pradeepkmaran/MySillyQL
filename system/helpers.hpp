// #include "imports.hpp"

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