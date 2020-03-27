#include "operation.h"

#ifndef INDEXSCAN_CLASS
#define INDEXSCAN_CLASS 1

class IndexScan : public Operation{
    public:
        IndexScan(Table *table, Index* index, int len);

        void print() {
            printf("IndexScan Table: %s, Index: %s, %d/%d , %d kids\n", table->getTableName(), index->getName(), len, index->getColNumber(), this->children.size());
            for(auto kid : children) {
                kid->print();
            }
        }

    private:
        Index* index;
        int len;
};

#endif