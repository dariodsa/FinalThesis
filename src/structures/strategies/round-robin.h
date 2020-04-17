#include "load-balance.h"

#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

class Database;

class RoundRobin : public LoadBalance{
    public:
        RoundRobin(std::vector<Database*> replicas);
        void addQuery(std::pair<const char*, int> query);
        void addInsertQuery(const char* query);
    private:
        int last;
};

#endif