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


        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
        Index *getIndex();
        bool getRetrData();
        bool isIndexCon() { return true; }

    private:
        Index* index;
        int len;
        bool retr_data;
        
};

#endif