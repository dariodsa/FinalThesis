#include "operation.h"
#include "../../db/program.h"

#ifndef FILTER_H
#define FILTER_H

class Filter : public Operation{
    public:
        Filter(bool isEqual, int numOfOperations);
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
    private:
        bool isEqual;
        int numOfOperations;
};

#endif