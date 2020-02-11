#include <string.h>

#ifndef COLUMN_H
#define COLUMN_H
#define MAX_LEN 30

class Column{
    public:
        Column(const char* name, const char* type);
        char* getName();
        char* getType();
        bool getPrimaryOrUnique();
        
        void setPrimaryOrUnique(bool value);
        
    private:
        char name[MAX_LEN];
        char type[MAX_LEN];
        bool primary_key_or_unique;
};

#endif