#ifndef OPERATION_H
#define OPERATION_H

#include "../table.h"

class Operation{
    public:
        Operation();
        Operation(Table* table);

        void addChild(Operation* child);
        
        float getTotalCost(Database* database) {
            float startCost = getStartCost(database);
            float runTimeCost = getRuntimeCost(database);
            printf("Start %f Runtime %f\n", startCost, runTimeCost);
            return startCost + runTimeCost;
        }

        void print();
        virtual double getStartCost(Database* database) = 0;
        virtual double getRuntimeCost(Database* database) = 0;
        virtual double getNt() = 0;

        std::vector<Operation*> children;
        Table *table;
    private:
        
        
};

class Dummy : public Operation{
    public:
    Dummy() : Operation() {}
    double getNt() {
        return 1;
    }
    double getStartCost(Database* database) {
        return 0;
    }
    double getRuntimeCost(Database* database) {
        return 0;
    }
};

#endif