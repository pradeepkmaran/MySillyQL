#include"row.hpp"

int main() {
    Schema sc = Schema("test_table");
    sc.addColumn(ColumnDef("name", FieldType::TEXT, 20));
    sc.addColumn(ColumnDef("reg", FieldType::INTEGER));

    string myName = "Pradeep";
    int64_t myReg = 92;

    Value name(myName);
    Value reg(myReg);

    Row ro(&sc);
    ro.setValue("name", name);
    ro.setValue("reg", reg);

    try {
        cout << ro.getValue("name").getText() << " " ;
        cout << ro.getValue("reg").getInt();
    } catch (exception e) {
        cout << e.what();
    }
}