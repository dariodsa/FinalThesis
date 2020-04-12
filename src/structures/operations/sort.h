#include "operation.h"
#include "../../db/program.h"

#ifndef SORT_H
#define SORT_H

class Sort : public Operation{
    public:
        Sort();
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
        
    private:
    
};

#endif