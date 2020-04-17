#include "load-balance.h"
#include "../database.h"

LoadBalance::LoadBalance(std::vector<Database*> replicas) {
    for(Database* database : replicas) {
        printf("ADDED %d\n", database);
        this->replicas.push_back(database);
    }
}