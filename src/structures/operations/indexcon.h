#ifndef INDEXCON_H
#define INDEXCON_H

#include "operation.h"

class IndexCon : public Operation{
    public:
        IndexCon(Table *table, Index* index, int len, bool retr_data);

        void print() {
            printf("IndexCon Table: %s, Index: %s, %d/%d , %d kids %d retr_data\n", table->getTableName(), index->getName(), len, index->getColNumber(), this->children.size(), retr_data);
            for(auto kid : children) {
                kid->print();
            }
        }

        float myCost(Database* database) {
            float cost = index->getCost(database, len, false);
            if(retr_data) {
                float table_cost = table->getCost(database, false);
                cost += table_cost;
            }
            return cost;
        }

        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);

    private:
        Index* index;
        int len;
        bool retr_data;
        
};

#endif