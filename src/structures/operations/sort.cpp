#include "sort.h"

Sort::Sort() {}

float Sort::getStartCost(Database* database) {
    float nt = getNt();
    float start_cost = 2.0 * database->CPU_OPERATOR_COST * nt * log(nt);
    for(Operation* child : children) {
        start_cost += child->getTotalCost(database);
    }
    return start_cost;
}

float Sort::getRuntimeCost(Database* database) {
    return database->CPU_TUPLE_COST * getNt();
}

float Sort::getNt() {
    return this->children[0]->getNt();
}