#include "filter.h"

Filter::Filter(int numOfOperations) {
    this->numOfOperations = numOfOperations;
}

float Filter::getStartCost(Database* database) {
    return 0;
}

float Filter::getRuntimeCost(Database* database) {
    return 0;
}

float Filter::getNt() {
    float nt = children[0]->getNt();
    if(1 == 1) nt /= 10;
    else nt /= 5;
    return nt;
}