#include "filter.h"

Filter::Filter(int numOfOperations) {
    this->numOfOperations = numOfOperations;
}

double Filter::getStartCost(Database* database) {
    if(children.size() == 0) return 0;
    else {
        return children[0]->getStartCost(database);
    }
}

double Filter::getRuntimeCost(Database* database) {
    if(children.size() == 0) return 0;
    int nt = children[0]->getNt();
    double cost = nt * ( numOfOperations * database->CPU_OPERATOR_COST + database->CPU_TUPLE_COST);
    return cost;
}

double Filter::getNt() {
    double nt = children[0]->getNt();
    nt /= 6;
    
    return nt;
}