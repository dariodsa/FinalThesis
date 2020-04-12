#include "toposort.h"
#include <algorithm>
#include <set>
#include <string>

#include "../structures/operations/operation.h"
#include "../structures/operations/nested-join.h"
#include "../structures/operations/hash-join.h"
#include "../structures/operations/merge-join.h"

using namespace std;

TopoSort::TopoSort(Database* database, Operation* parent) {
    this->database = database;
    this->parent = parent;
}
void TopoSort::addNode(char* t1) {
    string table_name = string(t1);
    
    if(M.find(table_name) == M.end()) {
        M[table_name]  = new vector<pair<string, pair<Link, bool> >>();
    } else if(RM.find(table_name) == RM.end()) {
        RM[table_name] = new vector<pair<string, pair<Link, bool> >>();
    }
}
void TopoSort::addLink(char* table_from, char* table_to, Link link, bool foreign) {
    printf("      LINK FROM %s_a TO %s_a %d\n", table_from, table_to, foreign);

    string str_table_from = string(table_from);
    string str_table_to = string(table_to);

    if(M.find(str_table_from) == M.end()) {
        M[str_table_from]  = new vector<pair<string, pair<Link, bool> >>();
    }
    if(RM.find(str_table_from) == RM.end()) {
        RM[str_table_from]  = new vector<pair<string, pair<Link, bool> >>();
    }
    
    if(M.find(str_table_to) == M.end()) {
        M[str_table_to]  = new vector<pair<string, pair<Link, bool> >>();
    }
    if(RM.find(str_table_to) == RM.end()) {
        RM[str_table_to]  = new vector<pair<string, pair<Link, bool> >>();
    }

    
    if(foreign) {
        if(foreigns.find(str_table_from) == foreigns.end()) {
            foreigns[str_table_from] = new vector<string>();
        }
        if(foreigns.find(str_table_to) == foreigns.end()) {
            foreigns[str_table_to] = new vector<string>();
        }
        foreigns[str_table_from]->push_back(table_to);
        return;

    }

    M[str_table_from]->push_back(make_pair(str_table_to, make_pair(link, foreign)));
    RM[str_table_to]->push_back(make_pair(str_table_from, make_pair(link, foreign)));
}

void TopoSort::setUpForeign(vector<expression_info*> area) {
    set<string> tables;
    for(expression_info* exp : area) {
        for(variable v : *(exp->variables)) {
            tables.insert(string(v.table));
        }
    }
    for(auto it1 = tables.begin(); it1 != tables.end(); ++it1) {
        for(auto it2 = tables.begin(); it2 != tables.end(); ++it2) {
            if(it1 == it2) continue;
            string table1 = *it1;
            string table2 = *it2;
            vector<char*> col1;
            vector<char*> col2;
            //try to find link from t1 to t2
            for(expression_info* exp : area) {
                string t1 = string((*exp->variables)[0].table);
                string t2 = string((*exp->variables)[1].table);

                if(exp->variables->size() != 2) continue;
                cout << t1 << " " << t2 << " search " << table1 << " " << table2 << endl;
                if(t1.compare(table1) == 0 && t2.compare(table2) == 0) {
                    printf("col1 %s col2 %s\n", (*exp->variables)[0].name, (*exp->variables)[1].name);
                    col1.push_back((*exp->variables)[0].name);
                    col2.push_back((*exp->variables)[1].name);
                }
            }
            char* t1 = (char*)table1.c_str();
            char* t2 = (char*)table2.c_str();
            
            if(database->isForeignKey(t1, col1, t2, col2)) {
                addLink(t1, t2, MERGE_JOIN, true);
            } else if(database->isForeignKey(t2, col2, t1, col1)) {
                addLink(t2, t1, MERGE_JOIN, true);
            }
        }
    }
}

Operation* TopoSort::performTopoSort(map<string, Operation*> *dictonary) {
    
    while(true) {
        vector<string> possible;

        int notVisited = 0;
        for(auto it = M.begin(); it != M.end(); ++it) {
            string node = it->first;
            if(visited[node] == true) continue;
            //nije visited
            ++notVisited;
            bool ok = true;
            cout << "RM " << node << endl;
            if(RM.find(node) != RM.end())
            for(auto pair : *RM[node]) {
                string op = pair.first;
                if(visited[op] == false) {
                    ok = false;
                    break;
                }
            }
            if(ok) possible.push_back(node);
        }
        if(notVisited == 0) break;

        if(possible.size() == 0 && notVisited > 0) { // RASPAŠOJ
            printf("RASPASOJ\n");
            for(auto it : *dictonary) {
                cout << "tables " << it.first;
                printf("a %d\n", it.second);
            }
            for(auto node : M) {
                if(visited[node.first] == false) {

                    NestedJoin* join = new NestedJoin();
                    join->addChild((*dictonary)[node.first]);
                    join->addChild(parent);
                    cout << node.first << " ";
                    printf("%d\n", (*dictonary)[node.first]);
                    parent = join;
                }
            }
            return parent;
        }

        string selected_node = possible[0];
        cout << "TOPO TABLE " << selected_node << " " << M.size() << endl;
        printf("%d %d\n", M.find(selected_node) == M.end(), RM.find(selected_node) == RM.end());
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