#pragma once
#include "../common/types.hpp"
#include "schema.hpp"
#include "value.hpp"

class Row {
private:
    map<string, Value> values;
    const Schema* schema;

public:
    explicit Row(const Schema* schema);
    
    void setValue(const string& columnName, const Value& value);
    Value getValue(const string& columnName) const;
    void serialize(void* destination) const;
    void deserialize(const void* source);
};