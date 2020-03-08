#include "result.h"
#include <queue>
using namespace std;

Select::Select(node *node) {
    if(node->terminal == true) {
        //easy
    }
    else {
        if(strcmp(node->name, NOT_STR) == 0) {
            //can't do anything right now, maybe de morgan later
        }
        else if(strcmp(node->name, AND_STR) == 0) {
            vector<expression_info> exp_list = this->bfs_populate(node);
        }
        else if(strcmp(node->name, OR_STR) == 0) {
            vector<expression_info> l1 = this->bfs_populate(node->left);
            vector<expression_info> l2 = this->bfs_populate(node->right);
        }
    }
}

vector<expression_info> Select::bfs_populate(node *n) {
    queue<node*> Q;
    vector<expression_info> list;

    Q.push(n);
    while(!Q.empty()) {
        node *pos = Q.front();
        Q.pop();

        if(pos->terminal) {
            list.push_back(*pos->e1);
            continue;
        }
        if(strcmp(pos->name, AND_STR) != 0) continue;
        //left
        if(pos->left != 0) {
            if(strcmp(pos->left->name, AND_STR) == 0) {
                Q.push(pos->left);
            }
        }

        //right
        if(pos->right != 0) {
            if(strcmp(pos->right->name, AND_STR) == 0) {
                Q.push(pos->right);
            }
        }
    }

    return list;
}