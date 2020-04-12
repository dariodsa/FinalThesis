#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "../structures/database.h"
#include <utility>

class Operation;

enum Link{
    NESTED_LOOP, HASH_JOIN, MERGE_JOIN
};

class TopoSort{
    public:
        TopoSort(Database* database, Operation* parent);
        void addNode(char* table_name);
        void addLink(char* table_from, char* table_to, Link link, bool foreign);
        void setUpForeign(std::vector<expression_info*> area);
        Operation* performTopoSort(std::map<std::string, Operation*> *dictonary);
    private:

        Database* database;
        Operation* parent;        
        
        std::map<std::string, std::vector<std::string>*> foreigns;

        std::map<std::string, std::vector<std::pair<std::string, std::pair<Link, bool>> >* >M;
        std::map<std::string, std::vector<std::pair<std::string, std::pair<Link, bool>> >* >RM;

        std::map<std::string, bool> visited;
};