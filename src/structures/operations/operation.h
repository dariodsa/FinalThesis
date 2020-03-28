#ifndef OPERATION_H
#define OPERATION_H

#include "../table.h"

class Operation{
    public:
        Operation(Table* table);
        void addChild(Operation* child);
        float getCost(Database* database);
        
        virtual void print();

        std::vector<Operation*> children;
        Table *table;
    private:
        
        virtual float myCost(Database* database);
};

#endif