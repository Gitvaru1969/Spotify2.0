#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

#define ARRAY_SIZE 8  // number of slots in each node's circular array

// NODE - one node in the doubly-linked list, holds a circular array of items
typedef struct node {
    void      **items;      // circular array of void* pointers
    int         count;      // number of items currently in this node
    int         first;      // index of the first (front) item
    struct node *next;      // pointer to next node toward the tail
    struct node *prev;      // pointer to prev node toward the head
} NODE;

// LIST - top-level structure with head/tail pointers and total item count
struct list {
    NODE *head;      // first node in the linked list
    NODE *tail;      // last node in the linked list
    int   numItems;  // total items across all nodes
};

// makeNode - allocate and initialize a new empty node // O(1)
static NODE *makeNode(void)
{
    NODE *np = malloc(sizeof(NODE));
    assert(np != NULL);

    np->items = malloc(sizeof(void *) * ARRAY_SIZE); // allocate circular array
    assert(np->items != NULL);

    np->count = 0;    // node starts empty
    np->first = 0;    // front starts at index 0
    np->next  = NULL;
    np->prev  = NULL;

    return np;
}

// createList - allocate and return a new empty list // O(1)
LIST *createList(void)
{
    LIST *lp = malloc(sizeof(LIST));
    assert(lp != NULL);

    lp->head     = NULL;
    lp->tail     = NULL;
    lp->numItems = 0;

    return lp;
}

// destroyList - free all nodes, their arrays, and the list struct // O(n)
void destroyList(LIST *lp)
{
    assert(lp != NULL);

    NODE *cur = lp->head;
    while (cur != NULL) {
        NODE *next = cur->next; // save next before freeing current
        free(cur->items);       // free the circular array
        free(cur);              // free the node itself
        cur = next;
    }

    free(lp); // free the list struct
}

// numItems - return the total number of items in the list // O(1)
int numItems(LIST *lp)
{
    assert(lp != NULL);
    return lp->numItems;
}

// addFirst - add item as the first element in the list // O(1)
void addFirst(LIST *lp, void *item)
{
    assert(lp != NULL);

    // if list is empty or head node is full, prepend a new node
    if (lp->head == NULL || lp->head->count == ARRAY_SIZE) {
        NODE *np = makeNode();

        if (lp->head == NULL) {
            lp->head = np; // list was empty: new node is head and tail
            lp->tail = np;
        } else {
            np->next       = lp->head; // prepend new node before current head
            lp->head->prev = np;
            lp->head       = np;
        }
    }

    NODE *front = lp->head;

    front->first = (front->first - 1 + ARRAY_SIZE) % ARRAY_SIZE; // move first back circularly
    front->items[front->first] = item; // store item at new front slot
    front->count++;

    lp->numItems++;
}

// addLast - add item as the last element in the list // O(1)
void addLast(LIST *lp, void *item)
{
    assert(lp != NULL);

    // if list is empty or tail node is full, append a new node
    if (lp->tail == NULL || lp->tail->count == ARRAY_SIZE) {
        NODE *np = makeNode();

        if (lp->tail == NULL) {
            lp->head = np; // list was empty: new node is head and tail
            lp->tail = np;
        } else {
            np->prev       = lp->tail; // append new node after current tail
            lp->tail->next = np;
            lp->tail       = np;
        }
    }

    NODE *back = lp->tail;

    int slot = (back->first + back->count) % ARRAY_SIZE; // next open slot
    back->items[slot] = item;
    back->count++;

    lp->numItems++;
}

// removeFirst - remove and return the first item in the list // O(1)
void *removeFirst(LIST *lp)
{
    assert(lp != NULL);
    assert(lp->numItems > 0); // list must not be empty

    // if head node is empty, free it and advance head
    if (lp->head->count == 0) {
        NODE *old = lp->head;
        lp->head  = old->next;

        if (lp->head != NULL)
            lp->head->prev = NULL; // new head has no prev
        else
            lp->tail = NULL;       // list became empty

        free(old->items);
        free(old);
    }

    NODE *front = lp->head;

    void *item   = front->items[front->first];        // grab item at front
    front->first = (front->first + 1) % ARRAY_SIZE;  // advance first circularly
    front->count--;

    lp->numItems--;

    // if head node is now empty and more nodes exist, free it
    if (front->count == 0 && front->next != NULL) {
        lp->head       = front->next;
        lp->head->prev = NULL;
        free(front->items);
        free(front);
    }

    return item;
}

// removeLast - remove and return the last item in the list // O(1)
void *removeLast(LIST *lp)
{
    assert(lp != NULL);
    assert(lp->numItems > 0); // list must not be empty

    // if tail node is empty, free it and retreat tail
    if (lp->tail->count == 0) {
        NODE *old = lp->tail;
        lp->tail  = old->prev;

        if (lp->tail != NULL)
            lp->tail->next = NULL; // new tail has no next
        else
            lp->head = NULL;       // list became empty

        free(old->items);
        free(old);
    }

    NODE *back = lp->tail;

    int slot   = (back->first + back->count - 1) % ARRAY_SIZE; // last item index
    void *item = back->items[slot];
    back->count--;

    lp->numItems--;

    // if tail node is now empty and more nodes exist, free it
    if (back->count == 0 && back->prev != NULL) {
        lp->tail       = back->prev;
        lp->tail->next = NULL;
        free(back->items);
        free(back);
    }

    return item;
}

// getItem - return item at position index, searching from the closer end // O(n/m)
void *getItem(LIST *lp, int index)
{
    assert(lp != NULL);
    assert(index >= 0 && index < lp->numItems);

    NODE *cur;

    if (index < lp->numItems / 2) {
        // search forward from head
        cur = lp->head;
        while (index >= cur->count) {
            index -= cur->count; // skip past this node's items
            cur    = cur->next;
        }
    } else {
        // search backward from tail
        int fromBack = lp->numItems - 1 - index; // distance from the back
        cur = lp->tail;
        while (fromBack >= cur->count) {
            fromBack -= cur->count; // skip past this node's items
            cur       = cur->prev;
        }
        index = cur->count - 1 - fromBack; // convert back to local index
    }

    return cur->items[(cur->first + index) % ARRAY_SIZE]; // map into circular array
}
