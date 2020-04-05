#include "operation.h"
#include "../../db/program.h"

#ifndef NESTED_JOIN_H
#define NESTED_JOIN_H

class Join;

class NestedJoin : public Operation{
    public:
        NestedJoin();
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
        
};

#endif