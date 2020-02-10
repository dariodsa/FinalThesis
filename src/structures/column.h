#include <string.h>

#ifndef COLUMN_H
#define COLUMN_H
#define MAX_LEN 30

class Column{
    public:
        Column(const char* name, const char* type);
        char* getName();
        char* getType();
    private:
        char name[MAX_LEN];
        char type[MAX_LEN];
};

#endif