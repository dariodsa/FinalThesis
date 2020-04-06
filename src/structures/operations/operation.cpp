#include "operation.h"

Operation::Operation() {}

Operation::Operation(Table *table) {
    this->table = table;
}

void Operation::addChild(Operation* chlid) {
    this->children.push_back(chlid);
}

void Operation::print() {
    printf("Operation %d kids\n", this->children.size());
    for(auto kid : children) kid->print();
}