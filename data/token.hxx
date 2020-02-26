#include <string.h>

struct variable {

    int depth;
    char name[100];
    variable(const char* _name, int _depth) {
        depth = depth;
        strcpy(name, _name);
    }
};

enum TYPE{
      SELECT_VARIABLE
    , CONDITION_VARIABLE
    , SORT_VARIABLE
    , GROUP_VARIABLE 
};