#include "operation.h"
#include "../../db/program.h"

#ifndef OR_UNION_H
#define OR_UNION_H

class OrUnion : public Operation{
    public:
        OrUnion();
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
    
};

#endif