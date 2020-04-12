#include "or-union.h"

OrUnion::OrUnion() {}

double OrUnion::getStartCost(Database* database) {
    double sum = 0;
    for(Operation* child : children) {
        sum += child->getStartCost(database);
    }
    return sum;
}

double OrUnion::getRuntimeCost(Database* database) {
    double sum = 0;
    double nt = getNt();
    for(Operation* child : children) {
        sum += child->getRuntimeCost(database);
    }
    sum += nt * (database->CPU_TUPLE_COST + database->CPU_OPERATOR_COST * 2);
    return sum;
}

double OrUnion::getNt() {
    if(children.size() == 0) return 0;
    double nt = 0;
    int numOfChildren = children.size();
    for(Operation* child : children) {
        nt += child->getNt();
    }
    return nt * ( 1.0 - (numOfChildren - 1) * 0.06);
}