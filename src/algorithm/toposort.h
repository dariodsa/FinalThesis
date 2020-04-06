#include <map>
#include <vector>
#include <utility>

class Operation;

enum Link{
    NESTED_LOOP, HASH_JOIN, MERGE_JOIN
};

class TopoSort{
    public:
        TopoSort(Operation* parent);
        void addNode(Operation* op);
        void addLink(Operation* from, Operation* to, Link link, bool foreign);
        Operation* performTopoSort();
    private:
        Operation* parent;

        std::map<Operation*, std::vector<std::pair<Operation*, std::pair<Link, bool>> >* >M;
        std::map<Operation*, std::vector<std::pair<Operation*, std::pair<Link, bool>> >* >RM;

        std::map<Operation*, bool> visited;
};