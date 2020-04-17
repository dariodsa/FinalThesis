#include "least-work.h"
#include "../result.h"

using namespace std;

LeastWork::LeastWork(std::vector<Database*> replicas) : LoadBalance(replicas) {

}
void LeastWork::addQuery(std::pair<const char*, int> query) {
    Database *best = replicas[0];
    int mini = replicas[0]->getNumOfActiveRequests();
    for(int i = 1, len = replicas.size(); i < len; ++i) {
        int curr_active = replicas[i]->getNumOfActiveRequests();
        if(curr_active < mini) {
            mini = curr_active;
            best = replicas[i];
        }
    }
    Select* select = process_query(this->replicas[0], query.first);
    select->setType(query.second);

    best->addRequest(select);
}
void LeastWork::addInsertQuery(const char* query) {

}