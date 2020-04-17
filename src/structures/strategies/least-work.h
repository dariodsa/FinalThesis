#include "load-balance.h"

#ifndef LEAST_WORK_H
#define LEAST_WORK_H


class LeastWork : public LoadBalance {
    public:
        LeastWork(std::vector<Database*> replicas);
        void addQuery(std::pair<const char*, int> query);
        void addInsertQuery(const char* query);

};

#endif