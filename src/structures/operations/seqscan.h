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
        
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();

    private:
        
};

#endif