#pragma once
#include "../common/types.hpp"
#include "../database/database.hpp"
#include "../storage/schema.hpp"

SillyResults executeCreateDatabase(Database& db, string &dbName);
SillyResults executeCreateTable(Schema &schema, Database &db);
SillyResults executeInsert(string tableName, vector<string> &rawValues, Database &db);
SillyResults executeSelect(string tableName, Database &db);