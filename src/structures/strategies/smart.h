#include "load-balance.h"

#ifndef SMART_H
#define SMART_H

class Smart : public LoadBalance{
    public:
        Smart(std::vector<Database*> replicas);
        void addQuery(std::pair<const char*, int> query);
        void addInsertQuery(const char* query);
    private:
        
};

#endif