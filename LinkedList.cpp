//*****************************************************************
//  LinkedList.cpp
//  HashTable
//
//  Created by Karlina Beringer on June 16, 2014.
//
//  This header file contains the Linked List class declaration.
//  Hash Table array elements consist of Linked List objects.
//*****************************************************************

#include "LinkedList.h"

// Constructs the empty linked list object.
// Creates the head node and sets length to zero.
LinkedList::LinkedList() {
    head = new Item;
    head -> next = NULL;
    length = 0;
}

// Inserts an item at the end of the list.
void LinkedList::insertItem(Item * newItem) {
    if (!head -> next)
    {
        head -> next = newItem;
        length++;
        return;
    }
    Item * p = head;
    Item * q = head;
    while (q)
    {
        p = q;
        q = p -> next;
    }
    p -> next = newItem;
    newItem -> next = NULL;
    length++;
}

bool LinkedList::existItem(long cost, int nbViolations3, int nbViolations4, int nbViolations5) {
    Item * p = head;
    Item * q = head;
    
    while(q) {
        p = q;
        if ((p != head)
            && (nbViolations3 == p->nbViolations3)
            && (nbViolations4 == p->nbViolations4)
            && (nbViolations5 == p->nbViolations5)
            && (cost == p->cost)) {
            return true;
        }
        q = p->next;
    }
    return false;
}

// Displays list contents to the console window.
void LinkedList::printList()
{
    if (length == 0)
    {
        cout << "\n{ }\n";
        return;
    }
    Item * p = head;
    Item * q = head;
    cout << "\n{ ";
    while (q)
    {
        p = q;
        if (p != head)
        {
            if (p -> next) cout << ", ";
            else cout << " ";
        }
        q = p -> next;
    }
    cout << "}\n";
}

// Returns the length of the list.
int LinkedList::getLength() {
    return length;
}

// De-allocates list memory when the program terminates.
LinkedList::~LinkedList() {
    Item * p = head;
    Item * q = head;
    while (q)
    {
        p = q;
        q = p -> next;
        if (q) delete p;
    }
}