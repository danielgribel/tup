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

/*returns cost delta if swap(u,v) on round s is performed*/
long Solution::umpireCostDelta(int u, int v, int s) {
	int nRounds = problemData.nRounds;
	int** dist = problemData.dist;
	long oldContrib = 0;
	long newContrib = 0;

	int v_left;
	int v_rigth;
	int v_center = scheduling[s][u].first;
	int v_new = scheduling[s][v].first;

	long dist_left_old = 0;
	long dist_rigth_old = 0;

	long dist_left_new = 0;
	long dist_rigth_new = 0;

	if(s == 0) {
		v_rigth = scheduling[s+1][u].first;
		dist_rigth_old = dist[v_center][v_rigth];
		dist_rigth_new = dist[v_new][v_rigth];
	} else if(s == nRounds-1) {
		v_left = scheduling[s-1][u].first;
		dist_left_old = dist[v_left][v_center];
		dist_left_new = dist[v_left][v_new];
	} else {
		v_left = scheduling[s-1][u].first;	
		v_rigth = scheduling[s+1][u].first;
		dist_left_old = dist[v_left][v_center];
		dist_rigth_old = dist[v_center][v_rigth];
		dist_left_new = dist[v_left][v_new];
		dist_rigth_new = dist[v_new][v_rigth];
	}
	oldContrib = dist_left_old + dist_rigth_old;
	newContrib = dist_left_new + dist_rigth_new;

	return newContrib - oldContrib;
}

/*returns the delta of violations on constraint 3 if swap(u,v) on round s is performed*/
int Solution::umpireConst3Delta(int u, int v, int s) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int* homeVisited = new int[nTeams];
	int venue;
	
	int nbViolationsOld = 0;
	int nbViolationsNew = 0;
	int contVenues = 0;

	for(int i = 0; i < nTeams; i++) {
		homeVisited[i] = 0;
	}
	for(int i = 0; i < nRounds; i++) {
		venue = scheduling[i][u].first;
		if(homeVisited[venue] == 0) {
			contVenues = contVenues + 1;
		}
		homeVisited[venue] = homeVisited[venue] + 1;
	}

	nbViolationsOld = nTeams - contVenues;

	if(homeVisited[scheduling[s][v].first] == 0 && homeVisited[scheduling[s][u].first] > 1) {
		contVenues++;
	}

	if(homeVisited[scheduling[s][v].first] >= 1 && homeVisited[scheduling[s][u].first] == 1) {
		contVenues--;
	}

	nbViolationsNew = nTeams - contVenues;
	delete[] homeVisited;

	return nbViolationsNew - nbViolationsOld;
}

/*returns the delta of violations on constraint 4 if swap(u,v) on round s is performed*/
int Solution::umpireConst4Delta(int u, int v, int s) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int timewindow4 = problemData.timewindow4;
	int* lastVisited = new int[nTeams];
	int venue;

	int nbViolationsOld = 0;
	int nbViolationsNew = 0;

	int limit_left = s - timewindow4 + 1;
	int limit_rigth = s + timewindow4;

	if(limit_left < 0) {
		limit_left = 0;
	}
	if(limit_rigth > nRounds) {
		limit_rigth = nRounds;
	}

	for(int i = 0; i < nTeams; i++) {
		lastVisited[i] = -1*timewindow4;
	}
	for(int i = limit_left; i < limit_rigth; i++) {
		venue = scheduling[i][u].first;
		if(i - lastVisited[venue] < timewindow4) {
			nbViolationsOld = nbViolationsOld + 1;
		}
		lastVisited[venue] = i;
	}

	for(int i = 0; i < nTeams; i++) {
		lastVisited[i] = -1*timewindow4;
	}
	for(int i = limit_left; i < limit_rigth; i++) {
		venue = scheduling[i][u].first;
		if(i == s) {
			venue = scheduling[i][v].first;
		}
		if(i - lastVisited[venue] < timewindow4) {
			nbViolationsNew = nbViolationsNew + 1;
		}
		lastVisited[venue] = i;
	}
	delete[] lastVisited;

	return nbViolationsNew - nbViolationsOld;
}

/*returns the delta of violations on constraint 5 if swap(u,v) on round s is performed*/
int Solution::umpireConst5Delta(int u, int v, int s) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int timewindow5 = problemData.timewindow5;
	int* lastSeen = new int[nTeams];
	int team1, team2;

	int nbViolationsOld = 0;
	int nbViolationsNew = 0;

	int limit_left = s - timewindow5 + 1;
	int limit_rigth = s + timewindow5;

	if(limit_left < 0) {
		limit_left = 0;
	}
	if(limit_rigth > nRounds) {
		limit_rigth = nRounds;
	}

	for(int i = 0; i < nTeams; i++) {
		lastSeen[i] = -1*timewindow5;
	}
	for(int i = limit_left; i < limit_rigth; i++) {
		team1 = scheduling[i][u].first;
		team2 = scheduling[i][u].second;
		if((i - lastSeen[team1] < timewindow5)) {
			nbViolationsOld = nbViolationsOld + 1;
		}
		if((i - lastSeen[team2] < timewindow5)) {
			nbViolationsOld = nbViolationsOld + 1;
		}
		lastSeen[team1] = i;
		lastSeen[team2] = i;
	}

	for(int i = 0; i < nTeams; i++) {
		lastSeen[i] = -1*timewindow5;
	}
	for(int i = limit_left; i < limit_rigth; i++) {
		team1 = scheduling[i][u].first;
		team2 = scheduling[i][u].second;
		if(i == s) {
			team1 = scheduling[i][v].first;
			team2 = scheduling[i][v].second;
		}
		if((i - lastSeen[team1] < timewindow5)) {
			nbViolationsNew = nbViolationsNew + 1;
		}
		if((i - lastSeen[team2] < timewindow5)) {
			nbViolationsNew = nbViolationsNew + 1;
		}
		lastSeen[team1] = i;
		lastSeen[team2] = i;
	}
	delete[] lastSeen;

	return nbViolationsNew - nbViolationsOld;
}

void Solution::swap(int u, int v, int s) {
	pii temp;
	temp.first = scheduling[s][u].first;
	temp.second = scheduling[s][u].second;

	scheduling[s][u] = scheduling[s][v];
	scheduling[s][v] = temp;
	
	calculateCost();
	calculateNbViolations3();
	calculateNbViolations4();
	calculateNbViolations5();
	calculateCorrectedCost();
}