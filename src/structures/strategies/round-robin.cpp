#include "round-robin.h"
#include "../result.h"

RoundRobin::RoundRobin(std::vector<Database*> replicas) : LoadBalance(replicas) {
    last = 0;
}
void RoundRobin::addQuery(std::pair<const char*, int> query) {
    
    if(last + 1 >= this->replicas.size()) last = 0;
    else ++last;
    printf("PROCESS %d\n", query.second);
    Select* select = process_query(this->replicas[0], query.first);

    select->setType(query.second);
    printf("%d/%d\n", last, replicas.size());
    this->replicas[last]->addRequest(select);
}

void RoundRobin::addInsertQuery(const char* query) {}