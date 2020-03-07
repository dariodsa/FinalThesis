#include <cpprest/json.h>
#include <string.h>

#ifndef MAX_LEN
#define MAX_LEN 50
#endif

#ifndef COLUMN_H
#define COLUMN_H

class Column{
    public:
        Column(const char* name, const char* type);
        Column(web::json::value value);
        char* getName();
        char* getType();
        bool getPrimaryOrUnique();
        
        void setPrimaryOrUnique(bool value);

        web::json::value getJSON();
        
    private:
        char name[MAX_LEN];
        char type[MAX_LEN];
        bool primary_key_or_unique;
};

#endif