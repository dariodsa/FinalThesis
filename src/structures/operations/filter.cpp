#include "filter.h"

Filter::Filter(bool isEqual, int numOfOperations) {
    printf("FILTER %d %d\n", isEqual, numOfOperations);
    this->isEqual = isEqual;
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
    double nt = children[0]->getNt();
    
    double cost = nt * ( numOfOperations * database->CPU_OPERATOR_COST + database->CPU_TUPLE_COST);
    return cost + children[0]->getTotalCost(database);
}

double Filter::getNt() {
    double nt = children[0]->getNt();
    if(isEqual) nt *= 0.01;
    else nt *= 0.07;
    if(nt < 1) nt = 1;
    return nt;
}