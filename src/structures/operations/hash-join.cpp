#include "hash-join.h"

HashJoin::HashJoin(bool foreign_key) {
    if(foreign_key) {
        this->nt = 0;
    } else {
        this->nt = -1;
    }
}

float HashJoin::getStartCost(Database* database) {
    return children[0]->getTotalCost(database) + children[1]->getStartCost(database);
}
float HashJoin::getRuntimeCost(Database* database) {
    return children[1]->getRuntimeCost(database) + children[1]->getNt() * (database->CPU_TUPLE_COST + database->CPU_OPERATOR_COST);
}

float HashJoin::getNt() {
    if(children.size() != 2) return 0;
    if(nt == -1) {
        return children[0]->getNt() * children[1]->getNt();
    } else {
        return children[0]->getNt();
    }
}