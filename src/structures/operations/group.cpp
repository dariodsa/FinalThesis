#include "group.h"

Group::Group(int numOfVariables) {
    this->numOfVariables = numOfVariables;
}

float Group::getStartCost(Database* database) {
    return 0;
}

float Group::getRuntimeCost(Database* database) {
    return 0;
}

float Group::getNt() {
    return 0;
}