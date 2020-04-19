#ifndef CACHE_H
#define CACHE_H

#include <string.h>
#include <stdlib.h>
#include <set>
#include "../db/program.h"

class Table;
class Index;
class Database;
struct Node;

class Cache{
    public:
        Cache(Database* database);
        void addNode(Table* table, bool full);
        void addNode(Index* index);
        long long getSize(Table* table);
        float getRatio(Index* index);
    private:
        void incrementTable(Node node);
        void removeOld();

        void insertNode(Node node);
        void deleteNode(Node node);
        void reduceRatio(Node* node);

        Database* database;
        std::set<Node>nodes;

        long long total_size = 0;
        int time_idx;
};
#endif