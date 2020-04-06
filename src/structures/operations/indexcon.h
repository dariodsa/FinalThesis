#ifndef INDEXCON_H
#define INDEXCON_H

#include "operation.h"

class IndexCon : public Operation{
    public:
        IndexCon(Table *table, Index* index, int len, bool retr_data);

        void print() {
            printf("IndexCon Table: %s, Index: %s, %d/%d , %d kids\n", table->getTableName(), index->getName(), len, index->getColNumber(), this->children.size());
            for(auto kid : children) {
                kid->print();
            }
        }


        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();

    private:
        Index* index;
        int len;
        bool retr_data;
        
};

#endif