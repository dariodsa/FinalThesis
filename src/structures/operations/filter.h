#include "operation.h"
#include "../../db/program.h"

#ifndef FILTER_H
#define FILTER_H

class Filter : public Operation{
    public:
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
    
};

#endif