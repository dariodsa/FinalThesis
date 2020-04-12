#include "operation.h"

Operation::Operation() {}

Operation::Operation(Table *table) {
    this->table = table;
}

void Operation::addChild(Operation* child) {
    if(child == 0) {
        child->getNt();
    }
    this->children.push_back(child);    
}

void Operation::print() {
    printf("Operation %d kids\n", this->children.size());
    for(auto kid : children) kid->print();
}