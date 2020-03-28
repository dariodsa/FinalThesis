#include "operation.h"

#ifndef SEQSCAN_CLASS
#define SEQSCAN_CLASS 1

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
            float cost = table->getCost(database, true);
            return cost;
        }

    private:
        
};

#endif