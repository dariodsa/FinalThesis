#include "result.h"
#include <stdio.h>
#include <queue>
using namespace std;

char AND_STR[] = "AND";
char NOT_STR[] = "NOT";
char OR_STR[]  = "OR";

Select::Select(node *root) {

    root = this->de_morgan(root);
    printf("Root name: %s\n", root->name);
    /*if(root->terminal == true) {
        //easy
    }
    else {
        if(strcmp(root->name, NOT_STR) == 0) {
            //can't do anything right now, maybe de morgan later
        }
        else if(strcmp(root->name, AND_STR) == 0) {
            vector<expression_info> exp_list = this->bfs_populate(root);
        }
        else if(strcmp(root->name, OR_STR) == 0) {
            vector<expression_info> l1 = this->bfs_populate(root->left);
            vector<expression_info> l2 = this->bfs_populate(root->right);
        }
    }*/
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

node* Select::de_morgan(node* root) {
    
    if(strcmp(root->name, NOT_STR) == 0) {
        if(!root->left->terminal && strcmp(root->left->name, NOT_STR) != 0){
            node* n = new node();
            node* not1 = new node();
            node* not2 = new node();
            
            n->left = not1;
            n->right = not2;
            
            //set name
            if(strcmp(root->left->name, OR_STR) == 0) {
                strcpy(n->name, AND_STR);
            } else {
                strcpy(n->name, OR_STR);
            }
            strcpy(not1->name, NOT_STR);
            strcpy(not2->name, NOT_STR);
            
            //set children
            not1->left = root->left->left;
            not2->left = root->left->right;
            return n;
        } else if(!root->left->terminal && strcmp(root->left->name, NOT_STR) == 0) {
            //skip one not
            return root->left;
        } else if(root->left->terminal) {
            if(root->left->e1->equal != 0) {
                root->left->e1->equal = 3 - root->left->e1->equal;
            }
            return root->left;
        }
    } else {
        node* n1 = this->de_morgan(root->left);
        if(root->left != n1) {
            root->left = n1;
        }
        node* n2 = this->de_morgan(root->right);
        if(root->right != n2) {
            root->right = n2;
        }
    }
    return root;
}