#ifndef OPERATION_H
#define OPERATION_H

#include "../table.h"

class Operation{
    public:
        Operation();
        Operation(Table* table);

        void addChild(Operation* child);
        
        float getTotalCost(Database* database) {
            return getStartCost(database) + getRuntimeCost(database);
        }

        void print();
        virtual float getStartCost(Database* database) = 0;
        virtual float getRuntimeCost(Database* database) = 0;
        virtual float getNt() = 0;

        std::vector<Operation*> children;
        Table *table;
    private:
        
        
};

class Dummy : public Operation{
    public:
    Dummy() : Operation() {}
    float getNt() {
        return 0;
    }
    float getStartCost(Database* database) {
        return 0;
    }
    float getRuntimeCost(Database* database) {
        return 0;
    }
};

#endif