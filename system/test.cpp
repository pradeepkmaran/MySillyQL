#include "imports.hpp"
#include "constants.hpp"
#include "components.hpp"
#include "helpers.hpp"
#include "connectors.hpp"
#include "parser.hpp"

int main() {
    Schema sc = Schema("test_table");
    sc.addColumn(ColumnDef("name", FieldType::TEXT, 20));
    sc.addColumn(ColumnDef("reg", FieldType::INTEGER));

    Table table(sc, "silly.db");

    string myName = "Pradeep";
    int64_t myReg = 92;

    Value name(myName);
    Value reg(myReg);

    Row ro = table.createRow();

    ro.setValue("name", name);
    ro.setValue("reg", reg);

    table.insertRow(ro);

    myName = "Niranjan";
    myReg = 82;

    Value newname(myName);
    Value newreg(myReg);

    Row newro = table.createRow();

    newro.setValue("name", newname);
    newro.setValue("reg", newreg);

    table.insertRow(newro);

    vector<Row> rows = table.getRows();
    for(Row row: rows) {
        cout << row.getValue("name").getText() << " " << row.getValue("reg").getInt() << endl;
    }
}