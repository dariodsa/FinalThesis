#include <string.h>
#include <stdlib.h>

#ifndef MAX_LEN
#define MAX_LEN 50
#endif

class Cache{
    public:
        Cache() : usage_count(0), ref_count(0) {}
    private:
        int usage_count;
        int ref_count;
        char name[MAX_LEN];
};


class Clock{
    public:
        Clock(int size) : size(size) {
            arr = (Cache**)malloc(sizeof(Cache*) * size);
            for(size_t i = 0; i < size; ++i) {
                arr[i] = 0;
            }
        }
        void insert(Cache *cache);
    private:
        Cache** arr;
        size_t current;
        size_t size;
};