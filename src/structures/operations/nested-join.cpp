#include "nested-join.h"

NestedJoin::NestedJoin(bool foreign_key) : Operation() {
    if(foreign_key) {
        this->nt = 0;
    } else {
        this->nt = -1;
    }

}

float NestedJoin::getStartCost(Database* database) {
    float start_cost = children[0]->getStartCost(database) + children[1]->getStartCost(database);
    return start_cost;
}

float NestedJoin::getRuntimeCost(Database* database) {
    return database->CPU_TUPLE_COST * children[0]->getNt() * children[1]->getNt() 
    + children[0]->getNt() * children[1]->getRuntimeCost(database);
}

float NestedJoin::getNt() {
    if(children.size() != 2) return 0;
    if(nt == -1) {
        return children[0]->getNt() * children[1]->getNt();
    } else {
        return children[0]->getNt();
    }
}