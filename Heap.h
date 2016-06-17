#ifndef Heap_Pdi_H
#define Heap_Pdi_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <functional>

class HeapPdi {
    private:
        std::vector< std::pair<long, int> > heap;
    public:
        HeapPdi();
        ~HeapPdi();
        void push_max(long cost, int val);
        int pop_max();
        void push_min(long cost, int val);
        int pop_min();
        std::vector< std::pair<long, int> > getHeap() { return heap; };
        void setHeap(std::vector< std::pair<long, int> > aHeap);
        std::pair<long, int> front_max();
		std::pair<long, int> front_min();
};

#endif