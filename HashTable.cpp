//*****************************************************************
//  HashTable.cpp
//  HashTable
//
//  Created by Kar Beringer on June 18, 2014.
//
//  This header file contains the Hash Table class definition.
//  Hash Table array elements consist of Linked List objects.
//*****************************************************************

#include "HashTable.h"

// Constructs the empty Hash Table object.
HashTable::HashTable(int tableLength) {
    if (tableLength <= 0) {
        tableLength = 211;
    } 
    array = new LinkedList[tableLength];
    length = tableLength;
}

// Returns an array location for a given item key.
int HashTable::hash(Item* item) {
    long value = item->cost + 1*item->nbViolations3 + 2*item->nbViolations4 + 3*item->nbViolations5;
    return value % length;
}

// Adds an item to the Hash Table.
void HashTable::insertItem(Item * newItem) {
    int index = hash(newItem);
    array[index].insertItem(newItem);
}

bool HashTable::existItem(Item* item) {
    int index = hash(item);
    return array[index].existItem(item->cost, item->nbViolations3, item->nbViolations4, item->nbViolations5);
}

// Display the contents of the Hash Table to console window.
void HashTable::printTable() {
    cout << "\n\nHash Table:\n";
    for ( int i = 0; i < length; i++ ) {
        cout << "Bucket " << i + 1 << ": ";
        array[i].printList();
    }
}

// Prints a histogram illustrating the Item distribution.
void HashTable::printHistogram() {
    cout << "\n\nHash Table Contains ";
    cout << getNumberOfItems() << " Items total\n";
    for ( int i = 0; i < length; i++ )
    {
        cout << i + 1 << ":\t";
        for ( int j = 0; j < array[i].getLength(); j++ )
            cout << " X";
        cout << "\n";
    }
}

// Returns the number of locations in the Hash Table.
int HashTable::getLength() {
    return length;
}

// Returns the number of Items in the Hash Table.
int HashTable::getNumberOfItems() {
    int itemCount = 0;
    for ( int i = 0; i < length; i++ )
    {
        itemCount += array[i].getLength();
    }
    return itemCount;
}

// De-allocates all memory used for the Hash Table.
HashTable::~HashTable() {
    delete [] array;
}