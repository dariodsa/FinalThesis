#include "../table.h"

#ifndef OPERATION_CLASS
#define OPERATION_CLASS 1

class Operation{
    public:
        Operation(Table* table);
        void addChild(Operation* child);
        float getCost();
        
        virtual void print();

        std::vector<Operation*> children;
        Table *table;
    private:
        
        virtual float myCost();
};

#endif