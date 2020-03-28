#ifndef INDEXSCAN_H
#define INDEXSCAN_H

#include "operation.h"

class IndexScan : public Operation{
    public:
        IndexScan(Table *table, Index* index, int len, bool retr_data);

        void print() {
            printf("IndexScan Table: %s, Index: %s, %d/%d , %d kids %d retr_data\n", table->getTableName(), index->getName(), len, index->getColNumber(), this->children.size(), this->retr_data);
            for(auto kid : children) {
                kid->print();
            }
        }

        float myCost(Database* database) {
            float cost = index->getCost(database, len);
            if(retr_data) {
                float table_cost = table->getCost(database, false);
                cost += table_cost;
            }
            return cost;
        }

    private:
        Index* index;
        int len;
        bool retr_data;
};

#endif