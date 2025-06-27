#include "../src/common/types.hpp"
#include "../src/storage/schema.hpp"
#include "../src/storage/table.hpp"
#include "../src/storage/value.hpp"

int main() {
    Schema schema = Schema("test_table");
    schema.addColumn(ColumnDefinition("name", FieldType::TEXT));
    schema.addColumn(ColumnDefinition("id", FieldType::INTEGER));

    Table table(schema, "test.db");

    string testName = "Alice";
    int64_t testId = 42;

    Value nameValue(testName);
    Value idValue(testId);

    Row row = table.createRow();

    row.setValue("name", nameValue);
    row.setValue("id", idValue);

    table.insertRow(row);

    testName = "Bob";
    testId = 84;

    Value newNameValue(testName);
    Value newIdValue(testId);

    Row newRow = table.createRow();

    newRow.setValue("name", newNameValue);
    newRow.setValue("id", newIdValue);

    table.insertRow(newRow);

    vector<Row> rows = table.getRows();
    for(Row tableRow: rows) {
        cout << tableRow.getValue("name").getText() << " " << tableRow.getValue("id").getInt() << endl;
    }
    
    return 0;
}