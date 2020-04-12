#include "operation.h"
#include "../../db/program.h"

#ifndef MERGE_JOIN_H
#define MERGE_JOIN_H

class MergeJoin : public Operation{
    public:
        MergeJoin(bool foreign_key = false);
        void setSort1(bool sort1);
        void setSort2(bool sort2);
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
    private:
        bool sort1 = false;
        bool sort2 = false;
        int nt = 0;
    
};

#endif