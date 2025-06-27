#pragma once
#include "../common/types.hpp"

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
    explicit Value(int64_t val);
    explicit Value(double val);
    explicit Value(bool val);
    Value();
    Value(const string& val);
    
    ~Value();
    Value(const Value& other);
    Value& operator=(const Value& other);
    
    void printType() const;
    FieldType getType() const;
    
    int64_t getInt() const;
    double getFloat() const;
    bool getBool() const;
    string getText() const;
    
    void serialize(void* destination) const;
    static Value deserialize(const void* source, FieldType type, uint32_t size);
};