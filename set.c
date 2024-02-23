#include <stdio.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct {
    int elements[MAX_SIZE];
    int size;
} Set;

// Initialize an empty set
void initializeSet(Set *set) {
    set->size = 0;
}

// Check if the set is empty
bool isEmpty(Set *set) {
    return set->size == 0;
}

// Check if an element is in the set
bool contains(Set *set, int element) {
    for (int i = 0; i < set->size; i++) {
        if (set->elements[i] == element) {
            return true;
        }
    }
    return false;
}

// Add an element to the set
void add(Set *set, int element) {
    if (!contains(set, element) && set->size < MAX_SIZE) {
        set->elements[set->size++] = element;
    }
}

// Remove an element from the set
void removeElement(Set *set, int element) {
    for (int i = 0; i < set->size; i++) {
        if (set->elements[i] == element) {
            for (int j = i; j < set->size - 1; j++) {
                set->elements[j] = set->elements[j + 1];
            }
            set->size--;
            return;
        }
    }
}

// Print the elements of the set
void printSet(Set *set) {
    printf("{ ");
    for (int i = 0; i < set->size; i++) {
        printf("%d ", set->elements[i]);
    }
    printf("}\n");
}

// int main() {
//     Set mySet;
//     initializeSet(&mySet);

//     add(&mySet, 5);
//     add(&mySet, 10);
//     add(&mySet, 15);
//     add(&mySet, 10); // Adding duplicate element

//     printf("Set contains 10: %s\n", contains(&mySet, 10) ? "true" : "false");
//     printf("Set contains 20: %s\n", contains(&mySet, 20) ? "true" : "false");

//     printf("Set before removal: ");
//     printSet(&mySet);

//     removeElement(&mySet, 10);
//     printf("Set after removal: ");
//     printSet(&mySet);

//     return 0;
// }
