#include "../table.h"

#ifndef OPERATION_CLASS
#define OPERATION_CLASS 1

class Operation{
    public:
        Operation(Table* table);
        void addChild(Operation* child);
        float getCost();
    private:
        std::vector<Operation*> children;
        Table *table;
        float myCost();
};

#endif