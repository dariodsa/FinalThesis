#include "nested-join.h"

NestedJoin::NestedJoin(bool foreign_key) : Operation() {
    printf("NESTED JOIN %d\n", foreign_key);
    if(foreign_key) {
        this->nt = 0;
    } else {
        this->nt = -1;
    }

}

double NestedJoin::getStartCost(Database* database) {
    if(children.size() != 2) return 0;
    
    double start_cost = children[0]->getStartCost(database) + children[1]->getStartCost(database);
    return start_cost;
}

double NestedJoin::getRuntimeCost(Database* database) {
    printf("call runtime nested\n");
    if(children.size() != 2) return 0;
    return database->CPU_TUPLE_COST * children[0]->getNt() * children[1]->getNt() 
    + children[1]->getRuntimeCost(database) + children[0]->getTotalCost(database);
}

double NestedJoin::getNt() {
    double nt1 = children[0]->getNt();
    double nt2 = children[1]->getNt();
    
    if(children.size() != 2) return 0;
    if(nt == -1) {
        return nt1 * nt2;
    } else {
        return nt1;
    }
}