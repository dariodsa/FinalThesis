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

float MergeJoin::getStartCost(Database* database) {
    if(children.size() != 2) return 0;
    float cost = children[0]->getStartCost(database) + children[1]->getStartCost(database);

    float N = children[0]->getNt();
    float M = children[1]->getNt();

    if(this->sort1 == false) cost += N * log2(N);
    if(this->sort2 == false) cost += M * log2(M);
    return cost;
}

float MergeJoin::getRuntimeCost(Database* database) {
    if(children.size() != 2) return 0;

    int N = children[0]->getNt();
    int M = children[1]->getNt();
    
    float cost = ( N + M) * (database->CPU_TUPLE_COST + database->CPU_OPERATOR_COST);
    return cost;
}

float MergeJoin::getNt() {
    if(children.size() != 2) return 0;
    if(nt == -1) {
        return children[0]->getNt() * children[1]->getNt();
    } else {
        return children[0]->getNt();
    }
}