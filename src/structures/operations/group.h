#include "operation.h"
#include "../../db/program.h"

#ifndef GROUP_H
#define GROUP_H

class Group : public Operation{
    public:
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
    private:
    
};

#endif