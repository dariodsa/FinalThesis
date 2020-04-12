#ifndef INDEXSCAN_H
#define INDEXSCAN_H

#include "operation.h"

class IndexScan : public Operation{
    public:
        IndexScan(Table *table, Index* index, int len, bool retr_data);

        void print() {
            printf("IndexScan Table: %s, Index: %s, %d/%d , %d kids\n", table->getTableName(), index->getName(), len, index->getColNumber(), this->children.size());
            for(auto kid : children) {
                kid->print();
            }
        }

        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();

    private:
        Index* index;
        int len;
        bool retr_data;
};

#endif