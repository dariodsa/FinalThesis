#include "smart.h"

using namespace std;


Smart::Smart(vector<Database*> replicas) : LoadBalance(replicas) {

}

void Smart::addQuery(std::pair<const char*, int> query) {

}
void Smart::addInsertQuery(const char* query) {

}