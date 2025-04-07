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

enum class PrepareResult { 
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_NEGATIVE_ID,
    PREPARE_UNRECOGNIZED_STATEMENT
};

enum class ExecuteResult { 
    EXECUTE_SUCCESS, 
    EXECUTE_TABLE_FULL ,
    EXECUTE_DUPLICATE_KEY
};

enum class StatementType { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
};