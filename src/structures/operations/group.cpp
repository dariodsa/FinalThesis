#include "group.h"

Group::Group(int numOfVariables) {
    this->numOfVariables = numOfVariables;
}

double Group::getStartCost(Database* database) {
    return children[0]->getStartCost(database);
}

double Group::getRuntimeCost(Database* database) {
    return children[0]->getRuntimeCost(database) + numOfVariables * children[0]->getNt() * (database->CPU_OPERATOR_COST + database->CPU_TUPLE_COST);
}

double Group::getNt() {
    return children[0]->getNt() / (numOfVariables + 1);
}