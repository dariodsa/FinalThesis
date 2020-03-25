#include "../table.h"

#ifndef OPERATION_CLASS
#define OPERATION_CLASS 1

class Operation{
    public:
        Operation(Table* table);
        void setChild(Operation* child);
        float getCost();
    private:
        Operation* child;
        Table *table;
};

#endif