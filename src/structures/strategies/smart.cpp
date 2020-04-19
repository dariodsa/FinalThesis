#include "smart.h"
#include "../result.h"
#include <chrono>

using namespace std;


Smart::Smart(vector<Database*> replicas) : LoadBalance(replicas) {

}

void Smart::addQuery(std::pair<const char*, int> query) {
    long long int curr_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

    Database *best = replicas[0];
    Select* select = process_query(this->replicas[0], query.first);
    select->setType(query.second);

    double select_cost = select->getFinalCost(replicas[0]);
    printf("    SELECT TYPE: %d, COST: %lf\n", query.second, select_cost);

    double mini = 0;
    
    for(int i = 0, len = replicas.size(); i < len; ++i) {
        
        double _mini = replicas[i]->totalCost + select_cost;
        pthread_mutex_lock(&(replicas[i]->mutex));
        if(replicas[i]->Q.size() > 0) {
            Select *_s = get<0>(replicas[i]->Q.front());
            double _cost = get<3>(replicas[i]->Q.front());
            double spent = curr_time - _s->getTimeStartProcess();
            _mini -= _cost / 1000;
            _mini += abs(_cost / 1000 - spent);
        }
        printf("     SELECT type %d, replica %d cost: %lf\n", query.second, i, _mini);
        pthread_mutex_unlock(&(replicas[i]->mutex));


        if(_mini < mini || i == 0) {
            mini = _mini;
            best = replicas[i];
        }
        if( _mini  < 2000) {
            best->addRequest(select);
            return;
        }
    }

    best->addRequest(select);
}
void Smart::addInsertQuery(const char* query) {

}