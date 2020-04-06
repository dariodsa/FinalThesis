#include "filter.h"

Filter::Filter(int numOfOperations) {
    this->numOfOperations = numOfOperations;
}

float Filter::getStartCost(Database* database) {
    if(children.size() == 0) return 0;
    else {
        return children[0]->getStartCost(database);
    }
}

float Filter::getRuntimeCost(Database* database) {
    if(children.size() == 0) return 0;
    int nt = children[0]->getNt();
    float cost = nt * ( numOfOperations * database->CPU_OPERATOR_COST + database->CPU_TUPLE_COST);
    return cost;
}

float Filter::getNt() {
    float nt = children[0]->getNt();
    nt /= 10;
    
    return nt;
}