#ifndef OPERATION_H
#define OPERATION_H

#include "../table.h"

class Operation{
    public:
        Operation(Table* table);

        void addChild(Operation* child);
        
        float getTotalCost(Database* database) {
            return getStartCost(database) + getRuntimeCost(database);
        }

        virtual void print() = 0;
        virtual float getStartCost(Database* database) = 0;
        virtual float getRuntimeCost(Database* database) = 0;

        std::vector<Operation*> children;
        Table *table;
    private:
        
        
};

#endif