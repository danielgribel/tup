#ifndef ProblemData_h
#define ProblemData_h

struct ProblemData {
    int nTeams;
    int nRounds;
    int n;
    int** dist;
    int** opponents;
    int timewindow4;
    int timewindow5;
    long penaltyRate;
};

#endif