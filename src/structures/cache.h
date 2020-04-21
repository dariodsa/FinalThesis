#ifndef CACHE_H
#define CACHE_H

#include <string.h>
#include <stdlib.h>
#include <set>
#include "../db/program.h"

#include "table.h"
#include "index.h"
class Database;
struct Node;


struct Node{
    int time_idx;
    double ratio;
    long long size;
    
    bool isIndex;
    Index* index;
    Table* table;
    Node() {}
    Node(Index* _index) {
        isIndex = true;
        index = _index;
        size = index->getSize();
        ratio = 1;
    }
    Node(Table* _table, bool full = false) {
        isIndex = false;
        table = _table;
        size = table->getSize();

        if(!full)size = size / 100000;
        
    }
    void setTime(int _time_id) {
        time_idx = _time_id;
    }
    long long getSize() {
        return size;
    }
    void reduce(long long diff) {
        size = size - diff;
    }
    
    bool operator <(const Node& A) const {
        if(isIndex == A.isIndex) {
            if(isIndex) {
                return index->getHash() < A.index->getHash();
            } else {
                return strcmp(table->getTableName(), A.table->getTableName()) < 0;
            }
        } else {
            return isIndex < A.isIndex;
        }
    }
};

struct node_pointer_cmp {
    bool operator()(const Node* A, const Node* B) const {
        if(A->isIndex == B->isIndex) {
            if(A->isIndex) {
                return A->index->getHash() < B->index->getHash();
            } else {
                return strcmp(A->table->getTableName(), B->table->getTableName()) < 0;
            }
        } else return A->isIndex < B->isIndex;
    }
};

class Cache{
    public:
        Cache(Database* database);
        void addNode(Table* table, bool full);
        void addNode(Index* index);
        long long getSize(Table* table);
        
    private:
        void incrementTable(Node* node);
        void removeOld();

        void insertNode(Node* node);
        void deleteNode(Node* node);
        void reduceRatio(Node* node);

        Database* database;
        std::set<Node*, node_pointer_cmp>nodes;

        long long total_size = 0;
        int time_idx;
};
#endif