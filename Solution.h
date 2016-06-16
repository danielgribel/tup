#ifndef Solution_h
#define Solution_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "ProblemData.h"

#define pii std::pair<int, int>

class Solution {
    private:
        pii** scheduling;
        long correctedCost;
        long cost;
        int nbViolations3;
        int nbViolations4;
        int nbViolations5;
        ProblemData problemData;
        void calculateCorrectedCost();
        void calculateCost();
        void calculateNbViolations3();
        void calculateNbViolations4();
        void calculateNbViolations5();

    public:        
        Solution(pii** scheduling, ProblemData problemData);
        ~Solution();
        pii** getScheduling();
        long getCorrectedCost();
        long getCost();
        int getNbViolations3();
        int getNbViolations4();
        int getNbViolations5();
        int getTotalViolations();
};

#endif