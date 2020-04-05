#include "or-union.h"

OrUnion::OrUnion() {}

float OrUnion::getStartCost(Database* database) {
    return 0;
}

float OrUnion::getRuntimeCost(Database* database) {
    return 0;
}

float OrUnion::getNt() {
    float nt = 0;
    for(Operation* child : children) {
        nt += child->getNt();
    }
    return nt * 0.92;
}