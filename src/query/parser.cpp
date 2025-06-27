#include "parser.hpp"
#include "executor.hpp"
#include "../common/utils.hpp"
#include "../io/input_output.hpp"

vector<string> tokenizeStatement(string *statement) {
    vector<string> tokens = splitString(statement, ' ');
    vector<string> fine_tokens;
    
    for (int token_index = 0; token_index < (int32_t)(tokens.size()); token_index++) {
        vector<string> delimits1 = splitString(&tokens[token_index], ',');
        for (string delimit1 : delimits1) {
            vector<string> delimits2 = splitString(&delimit1, ';');
            for (string delimit2 : delimits2) {
                fine_tokens.push_back(delimit2);
            }
        }
    }

    return fine_tokens;
}

SillyResults parseCreateDatabase(string* statement, Database& db, string &dbName) {
    vector<string> tokens = tokenizeStatement(statement);
    
    if (tokens.size() != 4 || tokens[0] != "silly" || tokens[1] != "make" || tokens[2] != "db") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }
    
    dbName = tokens[3];
    return SillyResults::SILLY_SUCCESS;
}

SillyResults parseCreateTable(string *statement, Schema &outSchema) {
    vector<string> tokens = tokenizeStatement(statement);

    if (tokens[0] != "silly" || tokens[1] != "make" || tokens[2] != "table" || tokens[4] != "with" || tokens[5] != "columns") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }

    outSchema = Schema(tokens[3]);

    for (int token_index=6; token_index < (int32_t)(tokens.size()); ) {
        string colName = tokens[token_index++];
        
        if (tokens[token_index++] != "of") return SillyResults::SILLY_SYNTAX_ERROR;

        FieldType fieldType = stringToFieldType(tokens[token_index++]);
        bool isPrimary = false;
        if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "as" && token_index++) {
            if (token_index < (int32_t)(tokens.size()) && tokens[token_index] == "primary" && token_index++) {
                isPrimary = true;
            } else if (token_index >= (int32_t)(tokens.size())) {
                return SillyResults::SILLY_SYNTAX_ERROR;
            }
        }
        outSchema.addColumn(ColumnDefinition(colName, fieldType, isPrimary));
    }

    return SillyResults::SILLY_SUCCESS;
}

SillyResults parseInsertStatement(string *statement, string &outTableName, vector<string> &outRawValues) {
    vector<string> tokens = tokenizeStatement(statement);
    if (tokens[0] != "silly" || tokens[1] != "put") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }
    
    outRawValues.clear();
    outTableName = "";
    
    for(int token_index=2; token_index<tokens.size(); token_index++) {
        if(tokens[token_index] == "inside") {
            if(token_index+1<tokens.size() && tokens[token_index+1] == "table") {
                if(token_index+3<tokens.size()) {
                    return SillyResults::SILLY_SYNTAX_ERROR;
                } else if(token_index+2<tokens.size()){
                    outTableName = tokens[token_index+2];
                    break;
                } else {
                    return SillyResults::SILLY_SYNTAX_ERROR;
                }
            } else {
                return SillyResults::SILLY_SYNTAX_ERROR;
            }
        } else {
            outRawValues.push_back(tokens[token_index]);
        }
    }

    return SillyResults::SILLY_SUCCESS;
}

SillyResults parseSelectStatement(string *statement, string &outTableName) {
    vector<string> tokens = tokenizeStatement(statement);
    return SillyResults::SILLY_SUCCESS;
}

SillyResults parseStatement(string *statement, Database &db) {
    if (statement->back() != ';') {
        return SillyResults::SILLY_SEMICOLON_MISSING_ERROR;
    }
    vector<string> tokens = tokenizeStatement(statement);
    if (tokens[0] != "silly") {
        return SillyResults::SILLY_SYNTAX_ERROR;
    }

    if (tokens[1] == "exit") { 
        closeInputBuffer(statement);
        cout << "Miss me please :(" << endl;
        exit(EXIT_SUCCESS);
        return SillyResults::SILLY_SUCCESS;
    } else if (tokens[1] == "make") {
        if(tokens[2] == "table") {
            Schema schema;
            SillyResults parseResult = parseCreateTable(statement, schema);
            if (parseResult != SillyResults::SILLY_SUCCESS) {
                return parseResult;
            }
            return executeCreateTable(schema, db);
        } else if (tokens[2] == "db") {
            string dbName;
            SillyResults parseResult = parseCreateDatabase(statement, db, dbName);
            if (parseResult != SillyResults::SILLY_SUCCESS) {
                return parseResult;
            }
            return executeCreateDatabase(db, dbName);
        } else {
            return SillyResults::SILLY_SYNTAX_ERROR;
        }
    } else if (tokens[1] == "put") {
        string tableName;
        vector<string> rawValues;
        SillyResults parseResult = parseInsertStatement(statement, tableName, rawValues);
        if (parseResult != SillyResults::SILLY_SUCCESS) {
            return parseResult;
        }
        return executeInsert(tableName, rawValues, db);
    } else if (tokens[1] == "get") {
        string tableName = tokens[2];
        SillyResults parseResult = parseSelectStatement(statement, tableName);
        if (parseResult != SillyResults::SILLY_SUCCESS) {
            return parseResult;
        }
        return executeSelect(tableName, db);
    } else if (tokens[1] == "save") {
        db.saveAllTables();
        return SillyResults::SILLY_SUCCESS;
    } else if (tokens[1] == "use") {
        db = Database(tokens[2]);
        db.loadAllTables();
        return SillyResults::SILLY_SUCCESS;
    } else {
        return SillyResults::SILLY_UNRECOGNIZED_STATEMENT;
    }
}