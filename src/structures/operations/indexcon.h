#include "operation.h"

#ifndef INDEXCON_CLASS
#define INDEXCON_CLASS 1

class IndexCon : public Operation{
    public:
        IndexCon(Table *table, Index* index, int len, bool retr_data);

        void print() {
            printf("IndexCon Table: %s, Index: %s, %d/%d , %d kids %d retr_data\n", table->getTableName(), index->getName(), len, index->getColNumber(), this->children.size(), retr_data);
            for(auto kid : children) {
                kid->print();
            }
        }
    private:
        Index* index;
        int len;
        bool retr_data;
        
};

#endif