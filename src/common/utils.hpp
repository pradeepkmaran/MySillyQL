#pragma once
#include "types.hpp"

class Value;

vector<string> splitString(string *str, char delimiter);
string fieldTypeToString(FieldType type);
FieldType stringToFieldType(string typeStr);
bool isValidValueForFieldType(const string& value, FieldType type);
Value stringToValue(const string& str, FieldType type);
void printTableBorder(const vector<size_t>& columnWidths);