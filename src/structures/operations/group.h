#include "operation.h"
#include "../../db/program.h"

#ifndef GROUP_H
#define GROUP_H

class Group : public Operation{
    public:
        Group(int numOfVariables);
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
    private:
        int numOfVariables;
    
};

#endif