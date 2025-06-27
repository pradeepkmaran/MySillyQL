#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <ctime>
using namespace std;

enum class FieldType {
    INTEGER,
    TEXT,
    DECIMAL,
    BOOLEAN,
    BLOB,
    UNKNOWN
};

enum class MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

enum SillyResults {
    SILLY_SUCCESS,
    SILLY_SYNTAX_ERROR,
    SILLY_SEMICOLON_MISSING_ERROR,
    SILLY_TYPE_MISMATCH_ERROR,
    SILLY_TABLE_NOT_FOUND_ERROR,
    SILLY_DB_NOT_FOUND_ERROR,
    SILLY_UNRECOGNIZED_STATEMENT,
    SILLY_EXECUTION_ERROR
};

enum class StatementType { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
};