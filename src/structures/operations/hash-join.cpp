#include "hash-join.h"

HashJoin::HashJoin(bool foreign_key) {
    
    if(foreign_key) {
        this->nt = 0;
    } else {
        this->nt = -1;
    }
}

double HashJoin::getStartCost(Database* database) {
    return children[0]->getTotalCost(database) + children[1]->getStartCost(database);
}
double HashJoin::getRuntimeCost(Database* database) {
    int nt1 = children[0]->getNt();
    int nt2 = children[1]->getNt();
    int my_nt = getNt();
    
    double cost = children[0]->getRuntimeCost(database) + children[1]->getRuntimeCost(database) + children[1]->getNt() * (database->CPU_TUPLE_COST + database->CPU_OPERATOR_COST);
    return cost;
}

double HashJoin::getNt() {
    if(children.size() != 2) return 0;
    if(nt == -1) {
        return children[0]->getNt() * children[1]->getNt();
    } else {
        return children[0]->getNt();
    }
}