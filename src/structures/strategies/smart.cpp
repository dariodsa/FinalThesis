#include "smart.h"
#include "../result.h"
#include <chrono>

using namespace std;

struct cmp_select_pointer { 
    bool operator()(Select* A, Select* B) 
    { 
        return A->_cost > B->_cost;
    } 
}; 


Smart::Smart(vector<Database*> replicas) : LoadBalance(replicas) {

}

void Smart::addQuery(std::pair<const char*, int> query) {
    long long int curr_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

    Database *best = replicas[0];
    printf("PROCESSING TYPE: %d\n", query.second);
    Select* select = process_query(this->replicas[0], query.first);
    select->setType(query.second);

    
    printf("    SELECT TYPE: %d\n", query.second);

    double mini = 0;
    
    for(int i = 0, len = replicas.size(); i < len; ++i) {
        double select_cost = select->getFinalCost(replicas[0]);
        select->_cost = select_cost;    

        double _mini = replicas[i]->totalCost + select_cost;
        pthread_mutex_lock(&(replicas[i]->mutex));
        //auto P = /*(priority_queue<Select*, vector<Select*>, cmp_select_pointer>*)*/ replicas[i]->Q2;
        if(replicas[i]->Q2.size() > 0) {
            Select *_s = replicas[i]->Q2.front();
            double _cost = _s->_cost;
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
        /*if( _mini  < 5000) {
            best->addRequest(select);
            return;
        }*/
    }

    best->addRequest(select);
}
void Smart::addInsertQuery(const char* query) {

}