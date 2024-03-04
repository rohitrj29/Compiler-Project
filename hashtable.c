#include "hashtable.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

// Hash function to determine the bucket index for a given key
unsigned int hash(char *key)
{
    unsigned int hash_value = 0;
    while (*key != '\0')
    {
        hash_value = (hash_value << 5) + *key++;
    }

    return hash_value % SIZE;
}

// Function to insert a key-value pair into the hashmap
void insertIntoHash(HashMap *map, char *key, char *value)
{
    unsigned int index = hash(key);

    // Check if the key already exists in the bucket
    Element *current = map->buckets[index];
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            // Key already exists, update the value
            current->value = value;
            return;
        }

        current = current->next;
    }

    // Key doesn't exist, create a new key-value pair
    Element *newPair = (Element *)malloc(sizeof(Element));
    newPair->key = strdup(key);
    newPair->value = value;
    newPair->next = map->buckets[index];
    map->buckets[index] = newPair;
}

// Function to initialize a new hashmap
HashMap *initializeHashMap()
{
    HashMap *myMap = (HashMap *)malloc(sizeof(HashMap));
    for (int i = 0; i < SIZE; i++)
    {
        myMap->buckets[i] = NULL;
    }

    // Inserting key-value pairs
    insertIntoHash(myMap, "with", "TK_WITH");
    insertIntoHash(myMap, "parameters", "TK_PARAMETERS");
    insertIntoHash(myMap, "end", "TK_END");
    insertIntoHash(myMap, "while", "TK_WHILE");
    insertIntoHash(myMap, "union", "TK_UNION");
    insertIntoHash(myMap, "endunion", "TK_ENDUNION");
    insertIntoHash(myMap, "definetype", "TK_DEFINETYPE");
    insertIntoHash(myMap, "as", "TK_AS");
    insertIntoHash(myMap, "type", "TK_TYPE");
    insertIntoHash(myMap, "_main", "TK_MAIN");
    insertIntoHash(myMap, "global", "TK_GLOBAL");
    insertIntoHash(myMap, "parameter", "TK_PARAMETER");
    insertIntoHash(myMap, "list", "TK_LIST");
    insertIntoHash(myMap, "input", "TK_INPUT");
    insertIntoHash(myMap, "output", "TK_OUTPUT");
    insertIntoHash(myMap, "int", "TK_INT");
    insertIntoHash(myMap, "real", "TK_REAL");
    insertIntoHash(myMap, "endwhile", "TK_ENDWHILE");
    insertIntoHash(myMap, "if", "TK_IF");
    insertIntoHash(myMap, "then", "TK_THEN");
    insertIntoHash(myMap, "endif", "TK_ENDIF");
    insertIntoHash(myMap, "read", "TK_READ");
    insertIntoHash(myMap, "write", "TK_WRITE");
    insertIntoHash(myMap, "return", "TK_RETURN");
    insertIntoHash(myMap, "call", "TK_CALL");
    insertIntoHash(myMap, "record", "TK_RECORD");
    insertIntoHash(myMap, "endrecord", "TK_ENDRECORD");
    insertIntoHash(myMap, "else", "TK_ELSE");

    return myMap;
}

// Function to search for a key in the hashmap
char *getValue(HashMap *map, char *key)
{
    unsigned int index = hash(key);
    Element *current = map->buckets[index];

    // Iterate through the linked list at the bucket
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            // Key found, return the corresponding value
            return current->value;
        }

        current = current->next;
    }

    // Key not found
    return "KEY NOT FOUND";
}

// Function to free the memory used by the hashmap
void destroyHashMap(HashMap *map)
{
    for (int i = 0; i < SIZE; i++)
    {
        Element *current = map->buckets[i];

        while (current != NULL)
        {
            Element *next = current->next;

            free(current->key);
            free(current);

            current = next;
        }
    }
    free(map);
}