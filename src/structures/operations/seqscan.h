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

        float myCost(Database* database) {
            int number_of_records = table->getNumOfRows();
            int relation_size = table->getSize();

            int blocks = relation_size / Program::BLOCK_SIZE;
            
            float cost = blocks * Program::SEQ_PAGE_COST
                        + number_of_records * Program::CPU_TUPLE_COST
                        + number_of_records * Program::CPU_FILTER_COST;

            printf("COST TABLE %f\n", cost);
            return cost;
        }

    private:
        
};

#endif