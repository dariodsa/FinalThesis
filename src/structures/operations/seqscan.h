#include "operation.h"
#include "../../db/program.h"

#ifndef SEQSCAN_H
#define SEQSCAN_H

class SeqScan : public Operation{
    public:
        SeqScan(Table *table);

        void print() {
            printf("SeqScan Table: %s, %d kids\n", table->getTableName(), this->children.size());
            for(auto kid : children) {
                kid->print();
            }
        }

        Table* getTable();
        
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();

    private:
        
};

#endif