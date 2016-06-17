#include "Heap.h"

HeapPdi::HeapPdi() {

}

HeapPdi::~HeapPdi() {

}

void HeapPdi::push_max(long cost, int val) {
    heap.push_back( std::pair<long, int>(cost, val) );
    push_heap(heap.begin(), heap.end());
}

int HeapPdi::pop_max() {
    make_heap(heap.begin(), heap.end());
    std::pair<long, int> pair = heap.front();
    pop_heap(heap.begin(), heap.end());
    heap.pop_back();
    return pair.second;
}

void HeapPdi::push_min(long cost, int val) {
    heap.push_back( std::pair<long, int>(cost, val) );
    push_heap(heap.begin(), heap.end(), std::greater< std::pair<long, int> >());
}

int HeapPdi::pop_min() {
    make_heap(heap.begin(), heap.end(), std::greater< std::pair<long, int> >());
    std::pair<long, int> pair = heap.front();
    pop_heap(heap.begin(), heap.end(), std::greater< std::pair<long, int> >());
    heap.pop_back();
    return pair.second;
}

std::pair<long, int> HeapPdi::front_max() {
    make_heap(heap.begin(), heap.end());
    return heap.front();
}

std::pair<long, int> HeapPdi::front_min() {
    make_heap(heap.begin(), heap.end(), std::greater< std::pair<long, int> >());
    return heap.front();
}

void HeapPdi::setHeap(std::vector< std::pair<long, int> > aHeap) {
    std::vector< std::pair<long, int> >(heap).swap(heap);
    heap = aHeap;
}