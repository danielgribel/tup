// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*
 
 This simple example shows how to call dlib's optimal linear assignment problem solver.
 It is an implementation of the famous Hungarian algorithm and is quite fast, operating in
 O(N^3) time.

*/

#include "MinAssignment.h"

std::vector<long> minAssignment(double** mat, int m) {
    // Let's imagine you need to assign N people to N jobs.  Additionally, each person will make
    // your company a certain amount of money at each job, but each person has different skills
    // so they are better at some jobs and worse at others.  You would like to find the best way
    // to assign people to these jobs.  In particular, you would like to maximize the amount of
    // money the group makes as a whole.  This is an example of an assignment problem and is
    // what is solved by the max_cost_assignment() routine.
    // 
    // So in this example, let's imagine we have 3 people and 3 jobs.  We represent the amount of
    // money each person will produce at each job with a cost matrix.  Each row corresponds to a
    // person and each column corresponds to a job.  So for example, below we are saying that
    // person 0 will make $1 at job 0, $2 at job 1, and $6 at job 2.
    
    //std::cout << "minAssignment" << std::endl;

    dlib::matrix<double> cost(m,m);
    double max = 0.0;
    double minCost = 0.0;

    for(int i = 0; i < m; i++) {
        for(int j = 0; j < m; j++) {
            if(mat[i][j] > max) {
                max = mat[i][j];
            }
        }
    }

    for(int i = 0; i < m; i++) {
        for(int j = 0; j < m; j++) {
            cost(i,j) = max - mat[i][j];
        }
    }

    std::vector<long> assignment = max_cost_assignment(cost);

    for(unsigned int i = 0; i < assignment.size(); i++) {
        minCost = minCost + (max - cost(i, assignment[i]));
    }

    //std::cout << minCost << std::endl;
    
    return assignment;
}