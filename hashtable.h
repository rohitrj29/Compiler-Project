#ifndef HASHTABLE_H
#define HASHTABLE_H

#define SIZE 10000

// Define a structure for a key-value pair
typedef struct KeyValuePair {
    char *key;
    char *value;
    struct KeyValuePair *next;
} Element;

// Define the hashmap structure
typedef struct HashMap {
    struct KeyValuePair *buckets[SIZE];
} HashMap;

unsigned int hash(char *key);
void insertIntoHash(HashMap *map, char *key, char *value);
HashMap *initializeHashMap();
char *getValue(HashMap *map, char *key);
void destroyHashMap(HashMap *map);

#endif