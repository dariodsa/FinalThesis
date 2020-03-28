#include "cache.h"
#include "database.h"
#include "index.h"
#include "table.h"

struct Node{
    int time_idx;
    float ratio;
    signed int size;
    
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
            ratio = Program::DEFAULT_TABLE_FETCH_SIZE;
        }
    }
    void setTime(int _time_id) {
        time_idx = _time_id;
    }
    signed int getSize() const {
        return (signed int) ((float)size * ratio);
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
    node.setTime(++time_idx);
    nodes.insert(node);
}

void Cache::addNode(Table* table, bool full) {
    Node node = Node(table, full);
    if(getLoadedStatus(table) != 0) {
        //increment table
        //remove old 
    } else {
        //add node in nodes

        //remove old
    }
}   

signed int Cache::getLoadedStatus(Table* table) {
    Node node = Node(table);
    auto iterator = nodes.find(node);
    if(iterator != nodes.end()) {
        //found
        
        return iterator->getSize();
    } else {
        //not found
        return 0;
    }
}

signed int Cache::getLoadedStatus(Index* index) {
    Node node = Node(index);
    auto iterator = nodes.find(node);
    if(iterator != nodes.end()) {
        //found
        return iterator->getSize();
    } else {
        //not found
        return 0;
    }
}

void Cache::incrementTable(Node node) {
    node.ratio += Program::DEFAULT_TABLE_FETCH_SIZE;
    if(node.ratio >= 1.00) {
        node.ratio = 1.00;
    }
}

void Cache::removeOld() {
    int max_size = database->getCacheSize();
    
    while(total_size >= max_size) {
        int s = MAX_INTEGER;
        Node max_node;
        for(Node node : nodes) {
            if(s > node.time_idx) {
                s = node.time_idx;
                max_node = node;
            }
        }
        if(max_node.isIndex) {
            deleteNode(max_node);
        } else {
            reduceRatio(max_node);
        }
    }
}

void Cache::insertNode(Node node) {
    total_size += node.getSize();
    nodes.insert(node);
    return;
}
void Cache::deleteNode(Node node) {
    total_size -= node.getSize();
    nodes.erase(node);
    return;
}

void Cache::reduceRatio(Node node) {

    float fetch_size = Program::DEFAULT_TABLE_FETCH_SIZE;
    
    if(node.ratio < 0.001) {
        deleteNode(node);
        return;
    }

    total_size -= node.getSize();
    node.ratio -= fetch_size;
    total_size += node.getSize();
}