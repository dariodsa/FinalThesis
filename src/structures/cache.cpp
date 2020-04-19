#include "cache.h"
#include "database.h"
#include "index.h"
#include "table.h"

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

        if(full) ratio = 1;
        else {
            size = size / 100000;
        }
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


Cache::Cache(Database* database) {
    this->database = database;
    this->time_idx = 0;
    this->total_size = 0;
}

void Cache::addNode(Index* index) {
    Node node = Node(index);
    node.isIndex = true;
    node.setTime(++time_idx);
    total_size += node.size;
    nodes.insert(node);
    removeOld();
}

void Cache::addNode(Table* table, bool full) {
    Node node = Node(table, full);
    if(getSize(table) != 0) {
        incrementTable(node);
    } else {
        //add node in nodes
        node.setTime(++time_idx);
        insertNode(node);
    }
    removeOld();
}   

long long Cache::getSize(Table* table) {
    Node node = Node(table);
    auto iterator = nodes.find(node);
    if(iterator != nodes.end()) {
        //found
        return iterator->size;
    } 
    return 0;
}

float Cache::getRatio(Index* index) {
    Node node = Node(index);
    auto iterator = nodes.find(node);
    if(iterator != nodes.end()) {
        //found
        return iterator->ratio;
    } else {
        //not found
        return 0;
    }
}

void Cache::incrementTable(Node node) {
    
    long long fetch_size = 10 * database->BLOCK_SIZE;
    
    long long t = node.size;

    node.size += fetch_size;
    node.size = max(node.size, node.table->getSize());
    
    
    total_size += node.size - t;

    node.setTime(++time_idx);
    removeOld();
}

void Cache::removeOld() {
    long long max_size = database->getCacheSize();
    
    int it = 0;
    
    while(total_size >= max_size) {
        ++it;
        int s = MAX_INTEGER;
        Node* max_node;
        
        for(Node node : nodes) {
            if(s > node.time_idx) {
                s = node.time_idx;
                max_node = &node;
            }
        }
        if(max_node->isIndex) {
            deleteNode(*max_node);
        } else {
            reduceRatio(max_node);
        }

        //if(it > 100) break;
    }
    printf("TOTAL SIZE %lld MAX_SITE %lld\n", total_size, max_size);
}

void Cache::insertNode(Node node) {
    
    total_size += node.getSize();
    nodes.insert(node);
    printf("DODAJEM %s %lld %lld\n", node.table, total_size, database->getCacheSize());
    removeOld();
    return;
}
void Cache::deleteNode(Node node) {
    total_size -= node.getSize();
    nodes.erase(node);
    printf("BRIŠEM %s %lld %lld\n", node.table, total_size, database->getCacheSize());
    return;
}

void Cache::reduceRatio(Node* _node) {
    
    auto node =(Node*) (nodes.find(*_node));
    
    float fetch_size = 400 * database->BLOCK_SIZE;
    
    if(node->size <= fetch_size) {
        deleteNode(*node);
        return;
    }
    
    total_size -= fetch_size;
    node->reduce(fetch_size);
    printf("REDUCE RATION %lld %lld %s\n", total_size, node->size, node->table->getTableName());
}