#include "cache.h"
#include "database.h"
#include "index.h"
#include "table.h"


Cache::Cache(Database* database) {
    this->database = database;
    this->time_idx = 0;
    this->total_size = 0;
}

void Cache::addNode(Index* index) {
    Node* node = new Node(index);
    node->isIndex = true;
    node->setTime(++time_idx);
    total_size += node->size;
    nodes.insert(node);
    removeOld();
}

void Cache::addNode(Table* table, bool full) {
    Node* node = new Node(table, full);
    if(getSize(table) != 0) {
        incrementTable(node);
    } else {
        //add node in nodes
        node->setTime(++time_idx);
        insertNode(node);
    }
    removeOld();
}   

long long Cache::getSize(Table* table) {
    Node* node = new Node(table);
    auto iterator = nodes.find(node);
    if(iterator != nodes.end()) {
        //found
        return (*iterator)->size;
    } 
    return 0;
}

void Cache::incrementTable(Node* node) {
    
    long long fetch_size = 10 * database->BLOCK_SIZE;
    
    long long t = node->size;

    node->size += fetch_size;
    node->size = max(node->size, node->table->getSize());
    
    
    total_size += node->size - t;

    node->setTime(++time_idx);
    removeOld();
}

void Cache::removeOld() {
    long long max_size = database->getCacheSize();
    
    total_size = 0;
    for(Node* node : nodes)  total_size += node->size;

    while(total_size >= max_size) {
        
        int s = MAX_INTEGER;
        Node* max_node = 0;
        //printf("Nodes size: %d %lld %lld\n", nodes.size(), total_size, max_size);
        int it = 0;    
        total_size = 0;
        for(Node* node : nodes) {
            
            total_size += node->size;
            if(s > node->time_idx) {
                s = node->time_idx;
                max_node = node;
            }
        }
        if(nodes.size() == 0) continue;
        if(max_node->isIndex) {
            deleteNode(max_node);
        } else {
            reduceRatio(max_node);
        }

        //if(it > 100) break;
    }
    //printf("TOTAL SIZE %lld MAX_SITE %lld\n", total_size, max_size);
}

void Cache::insertNode(Node* node) {
    
    total_size += node->getSize();
    nodes.insert(node);
    //printf("DODAJEM %s %lld %lld\n", node->table, total_size, database->getCacheSize());
    removeOld();
    return;
}
void Cache::deleteNode(Node* node) {
    total_size -= node->getSize();
    nodes.erase(node);
    return;
}

void Cache::reduceRatio(Node* _node) {
    
    float fetch_size = 400 * database->BLOCK_SIZE;
    
    if(_node->size <= fetch_size) {
        deleteNode(_node);
        return;
    }
    
    total_size -= fetch_size;
    _node->reduce(fetch_size);
    
}