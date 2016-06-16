#include "Solution.h"

Solution::Solution(pii** _scheduling, ProblemData _problemData) {
	scheduling = _scheduling;
	problemData =  _problemData;
	calculateCost();
	calculateNbViolations3();
	calculateNbViolations4();
	calculateNbViolations5();
	calculateCorrectedCost();
}

Solution::~Solution() {
	for(int i = 0; i < problemData.nRounds; i++) {
        delete [] scheduling[i];
    }
    delete [] scheduling;
}

pii** Solution::getScheduling() {
	return scheduling;
}

long Solution::getCorrectedCost() {
	return correctedCost;
}

long Solution::getCost() {
	return cost;
}

int Solution::getNbViolations3() {
	return nbViolations3;
}

int Solution::getNbViolations4() {
	return nbViolations4;
}

int Solution::getNbViolations5() {
	return nbViolations5;
}

int Solution::getTotalViolations() {
	return nbViolations3 + nbViolations4 + nbViolations5;
}

void Solution::calculateCorrectedCost() {
	correctedCost = getCost() + problemData.penaltyRate*getTotalViolations();
}

void Solution::calculateCost() {
	int** dist = problemData.dist;
	int nRounds = problemData.nRounds;
	int n = problemData.n;

	cost = 0;

	for(int i = 0; i < n; i++) {
		for(int j = 0; j < nRounds-1; j++) {
			cost = cost + dist[scheduling[j][i].first][scheduling[j+1][i].first];
		}
	}
}

// Feasibility test for constraint (3):
// Every umpire sees every team at least once at the team home
void Solution::calculateNbViolations3() {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int* homeVisited = new int[nTeams];
	int venue;
	int contVenues = 0;
	nbViolations3 = 0;
	
	for(int u = 0; u < n; u++) {
		for(int i = 0; i < nTeams; i++) {
			homeVisited[i] = 0;
		}
		for(int i = 0; i < nRounds; i++) {
			venue = scheduling[i][u].first;
			if(homeVisited[venue] == 0) {
				homeVisited[venue] = 1;
				contVenues = contVenues + 1;
			}
		}
	}
	nbViolations3 = nTeams*n - contVenues;
	delete[] homeVisited;
}

// Feasibility test for constraint (4):
// No umpire is in a home site more than once in any n - d1 consecutive slots
void Solution::calculateNbViolations4() {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int timewindow4 = problemData.timewindow4;
	int* lastVisited = new int[nTeams];
	int venue;
	nbViolations4 = 0;

	for(int u = 0; u < n; u++) {
		for(int i = 0; i < nTeams; i++) {
			lastVisited[i] = -1*timewindow4;
		}
		for(int i = 0; i < nRounds; i++) {
			venue = scheduling[i][u].first;
			if(i - lastVisited[venue] < timewindow4) {
				nbViolations4 = nbViolations4 + 1;
			}
			lastVisited[venue] = i;
		}
	}
	delete[] lastVisited;
}

// Feasibility test for constraint (5):
// No umpire sees a team more than once in any n/2 - d2 consecutive slots.
void Solution::calculateNbViolations5() {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int timewindow5 = problemData.timewindow5;
	int* lastSeen = new int[nTeams];
	int team1, team2;
	nbViolations5 = 0;

	for(int u = 0; u < n; u++) {
		for(int i = 0; i < nTeams; i++) {
			lastSeen[i] = -1*timewindow5;
		}
		for(int i = 0; i < nRounds; i++) {
			team1 = scheduling[i][u].first;
			team2 = scheduling[i][u].second;
			if((i - lastSeen[team1] < timewindow5)) {
				nbViolations5 = nbViolations5 + 1;
			}
			if((i - lastSeen[team2] < timewindow5)) {
				nbViolations5 = nbViolations5 + 1;
			}
			lastSeen[team1] = i;
			lastSeen[team2] = i;
		}
	}

	delete[] lastSeen;
}