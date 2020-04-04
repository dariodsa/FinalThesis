#include "foreign-key.h"
#include "table.h"
#include "column.h"

using namespace std;

ForeignKey::ForeignKey() {

}
void ForeignKey::setTable(const char *table1) {
    this->table1 = table1;
}

void ForeignKey::setTable2(const char *table2) {
    this->table2 = table2;
}

void ForeignKey::addColumnInTable1(char *column) {
    this->columns_table1.push_back(column);
}
void ForeignKey::addColumnInTable2(char *column) {
    this->columns_table2.push_back(column);
} 