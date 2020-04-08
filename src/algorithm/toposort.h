#include <map>
#include <string>
#include <vector>
#include <utility>

class Operation;

enum Link{
    NESTED_LOOP, HASH_JOIN, MERGE_JOIN
};

class TopoSort{
    public:
        TopoSort(Operation* parent);
        void addNode(char* table_name);
        void addLink(char* table_from, char* table_to, Link link, bool foreign);
        Operation* performTopoSort(std::map<std::string, Operation*> *dictonary);
    private:

        Operation* parent;        
        
        std::map<std::string, std::vector<std::pair<std::string, std::pair<Link, bool>> >* >M;
        std::map<std::string, std::vector<std::pair<std::string, std::pair<Link, bool>> >* >RM;

        std::map<std::string, bool> visited;
};