#include "operation.h"

#ifndef INDEXSCAN_CLASS
#define INDEXSCAN_CLASS 1

class IndexScan : public Operation{
    public:
        IndexScan(Table *table, Index* index, int len, bool retr_data);

        void print() {
            printf("IndexScan Table: %s, Index: %s, %d/%d , %d kids %d retr_data\n", table->getTableName(), index->getName(), len, index->getColNumber(), this->children.size(), this->retr_data);
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