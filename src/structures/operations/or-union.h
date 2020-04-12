#include "operation.h"
#include "../../db/program.h"

#ifndef OR_UNION_H
#define OR_UNION_H

class OrUnion : public Operation{
    public:
        OrUnion();
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
    private:
    
};

#endif