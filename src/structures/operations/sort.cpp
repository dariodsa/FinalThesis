#include "sort.h"

Sort::Sort() {}

double Sort::getStartCost(Database* database) {
    double nt = getNt();
    double start_cost = 2.0 * database->CPU_OPERATOR_COST * nt * log(nt);
    for(Operation* child : children) {
        start_cost += child->getTotalCost(database);
    }
    return start_cost;
}

double Sort::getRuntimeCost(Database* database) {
    return database->CPU_TUPLE_COST * getNt();
}

double Sort::getNt() {
    return this->children[0]->getNt();
}