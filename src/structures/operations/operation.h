#include "result.h"

class Operation{
    public:
        Operation(Table* table);
        void setChild(Operation* child);
    private:
        Operation* child;
        Table *table;
};