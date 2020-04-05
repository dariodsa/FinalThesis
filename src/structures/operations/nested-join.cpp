#include "nested-join.h"

float NestedJoin::getStartCost(Database* database) {
    float start_cost = children[0]->getStartCost(database) + children[1]->getStartCost(database);
    return start_cost;
}

float NestedJoin::getRuntimeCost(Database* database) {
    return database->CPU_TUPLE_COST * children[0]->getNt() * children[1]->getNt() 
    + children[0]->getNt() * children[1]->getRuntimeCost();
}