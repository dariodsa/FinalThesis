#include "sort.h"

Sort::Sort() {
    printf("SORT\n");
}

double Sort::getStartCost(Database* database) {
    double nt = getNt();
    double start_cost = 2.0 * database->CPU_OPERATOR_COST * nt * log(nt);
    for(Operation* child : children) {
        start_cost += child->getTotalCost(database);
    }
    return start_cost;
}

double Sort::getRuntimeCost(Database* database) {
    double nt = getNt();
    double child_cost = children[0]->getTotalCost(database);
    printf("sort child cost %lf\n", child_cost);
    return child_cost + (database->CPU_OPERATOR_COST) * nt * log2(nt);
}

double Sort::getNt() {
    return this->children[0]->getNt();
}