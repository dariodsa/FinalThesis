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
            return startCost + runTimeCost;
        }

        void print();
        virtual double getStartCost(Database* database) = 0;
        virtual double getRuntimeCost(Database* database) = 0;
        virtual double getNt() = 0;
        bool isSeqScan() { return false; }
        bool isIndexScan() { return false; }
        bool isIndexCon() { return false; }

        Table* getTable() { return this->table;}

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