#include "operation.h"

Operation::Operation(Table *table) {
    this->table = table;
}

void Operation::addChild(Operation* chlid) {
    this->children.push_back(chlid);
}

float Operation::myCost(Database* database) {
    return 1;
}

float Operation::getCost(Database *database) {
    float ans = 0;
    for(Operation* op : children) {
        ans += op->getCost(database);
    }
    return ans * myCost(database);
}

void Operation::print() {
    printf("Operation %d kids\n", this->children.size());
    for(auto kid : children) kid->print();
}