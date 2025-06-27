#pragma once
#include "../common/types.hpp"
#include "../database/database.hpp"
#include "../storage/schema.hpp"

vector<string> tokenizeStatement(string *statement);
SillyResults parseCreateDatabase(string* statement, Database& db, string &dbName);
SillyResults parseCreateTable(string *statement, Schema &outSchema);
SillyResults parseInsertStatement(string *statement, string &outTableName, vector<string> &outRawValues);
SillyResults parseSelectStatement(string *statement, string &outTableName);
SillyResults parseStatement(string *statement, Database &db);