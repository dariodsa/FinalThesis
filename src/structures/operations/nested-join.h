#include "operation.h"
#include "../../db/program.h"

#ifndef NESTED_JOIN_H
#define NESTED_JOIN_H

class NestedJoin : public Operation{
    public:
        NestedJoin(bool foreign_key = false);
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
        int nt;
};

#endif