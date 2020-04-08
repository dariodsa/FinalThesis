#include "toposort.h"
#include <algorithm>
#include "../structures/operations/operation.h"
#include "../structures/operations/nested-join.h"
#include "../structures/operations/hash-join.h"
#include "../structures/operations/merge-join.h"

using namespace std;

TopoSort::TopoSort(Operation* parent) {
    this->parent = parent;
}
void TopoSort::addNode(char* t1) {
    string table_name = string(t1);
    
    M[table_name]  = new vector<pair<string, pair<Link, bool> >>();
    RM[table_name] = new vector<pair<string, pair<Link, bool> >>();
}
void TopoSort::addLink(char* table_from, char* table_to, Link link, bool foreign) {
    
    M[table_from]->push_back(make_pair(table_to, make_pair(link, foreign)));
    RM[table_to]->push_back(make_pair(table_from, make_pair(link, foreign)));
}
Operation* TopoSort::performTopoSort(map<string, Operation*> *dictonary) {
    
    while(true) {
        vector<string> possible;

        for(auto it = M.begin(); it != M.end(); ++it) {
            string node = it->first;
            if(visited[node] == true) break;
            //nije visited
            bool ok = true;
            for(auto pair : *RM[node]) {
                string op = pair.first;
                if(visited[op] == false) {
                    ok = false;
                    break;
                }
            }
            if(ok) possible.push_back(node);
        }
        if(possible.size() == 0) { // RASPAŠOJ
            for(auto node : M) {
                if(visited[node.first] == false) {

                    NestedJoin* join = new NestedJoin();
                    join->addChild((*dictonary)[node.first]);
                    join->addChild(parent);
                    parent = join;
                }
            }
            return parent;
        }

        string selected_node = possible[0];
        visited[selected_node] = true;
        Link link = NESTED_LOOP;
        bool foreign = false;

        //link type
        for(auto pair : *RM[selected_node]) {
            Link _link = pair.second.first;
            if(_link == HASH_JOIN) {
                link = HASH_JOIN;
                break;
            } else if(_link == NESTED_LOOP) {
                link = NESTED_LOOP;
                break;
            } else if(_link == MERGE_JOIN) {
                link = MERGE_JOIN;
            }
        }
        //foreign type
        for(auto pair : *RM[selected_node]) {
            if(pair.second.second) foreign = true;
        }

        if(link == HASH_JOIN) {
            HashJoin* join = new HashJoin(foreign);
            join->addChild((*dictonary)[selected_node]);
            join->addChild(parent);
            parent = join;
        } else if(link == NESTED_LOOP || link == MERGE_JOIN) {
            NestedJoin* join = new NestedJoin(foreign);
            join->addChild((*dictonary)[selected_node]);
            join->addChild(parent);
            parent = join;
        }

    }
    return parent;
}