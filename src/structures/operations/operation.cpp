#include "operation.h"

Operation::Operation(Table *table) {
    this->table = table;
}

void Operation::addChild(Operation* chlid) {
    this->children.push_back(chlid);
}

float Operation::myCost() {
    return 1;
}

float Operation::getCost() {
    float ans = 0;
    for(Operation* op : children) {
        ans += op->getCost();
    }
    return ans * myCost();
}