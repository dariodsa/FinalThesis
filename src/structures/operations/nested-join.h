#include "operation.h"
#include "../../db/program.h"

#ifndef NESTED_JOIN_H
#define NESTED_JOIN_H

class NestedJoin : public Operation{
    public:
        NestedJoin(bool foreign_key = false);
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
    private:
        int nt;
};

#endif