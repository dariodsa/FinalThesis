#include "operation.h"
#include "../../db/program.h"

#ifndef GROUP_H
#define GROUP_H

class Group : public Operation{
    public:
        Group(int numOfVariables);
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
        int numOfVariables;
    
};

#endif