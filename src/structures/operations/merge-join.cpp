#include "merge-join.h"

MergeJoin::MergeJoin(bool foreign_key) {
    if(foreign_key) {
        this->nt = 0;
    } else {
        this->nt = -1;
    }
}

void MergeJoin::setSort1(bool sort1) {
    this->sort1 = sort1;
}

void MergeJoin::setSort2(bool sort2) {
    this->sort2 = sort2;
}

double MergeJoin::getStartCost(Database* database) {
    if(children.size() != 2) return 0;
    double cost = children[0]->getStartCost(database) + children[1]->getStartCost(database);

    double N = children[0]->getNt();
    double M = children[1]->getNt();

    if(this->sort1 == false) cost += N * log2(N);
    if(this->sort2 == false) cost += M * log2(M);
    return cost;
}

double MergeJoin::getRuntimeCost(Database* database) {
    if(children.size() != 2) return 0;

    int N = children[0]->getNt();
    int M = children[1]->getNt();
    
    double cost = ( N + M) * (database->CPU_TUPLE_COST + database->CPU_OPERATOR_COST);
    return cost;
}

double MergeJoin::getNt() {
    if(children.size() != 2) return 0;
    if(nt == -1) {
        return children[0]->getNt() * children[1]->getNt();
    } else {
        return children[0]->getNt();
    }
}