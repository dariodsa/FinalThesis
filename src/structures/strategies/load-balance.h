#include <vector>
#include <utility>
#include "../../db/config.h"

#ifndef LOAD_BALANCE_H
#define LOAD_BALANCE_H

class Database;

class LoadBalance{
    public:
        LoadBalance(std::vector<Database*> replicas);
        virtual void addQuery(std::pair<const char*, int> query) = 0;
        virtual void addInsertQuery(const char* query) = 0;
    protected:
        std::vector<Database*> replicas;
};

#endif