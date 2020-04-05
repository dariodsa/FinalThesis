#include "operation.h"
#include "../../db/program.h"

#ifndef SORT_H
#define SORT_H

class Sort : public Operation{
    public:
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
    private:
    
};

#endif