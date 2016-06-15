#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <map>
#include <ctime>
#include <cstdlib>
#include <set>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include <limits>
#include <queue>
#include <functional> 
#include <cctype>
#include <locale>

#define pii std::pair<int, int>

struct ProblemData {
	int nTeams;
	int nRounds;
	int n;
	int** dist;
	int** opponents;	
};

ProblemData problemData;

const double MAX_LONG = std::numeric_limits<long>::max();
const int timewindow4 = 4;
const int timewindow5 = 2;

bool isNum(char c) {
    return std::string("-0123456789 ").find(c) != std::string::npos;
}

std::string strip( const std::string &s ) {
    std::string result;
    result.reserve( s.length() );

    std::remove_copy_if( s.begin(),
                         s.end(),
                         std::back_inserter( result ),
                         std::not1( std::ptr_fun( isNum ) ) );

    return result;
}

bool bothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

void deleteMatrix(int** matrix, int n) {
	for(int i = 0; i < n; i++) {
        delete [] matrix[i];
    }
    delete [] matrix;
}

void deleteSolution(pii** solution) {
	int nRounds = problemData.nRounds;
	for(int i = 0; i < nRounds; i++) {
        delete [] solution[i];
    }
    delete [] solution;
}

long cost(pii** solution) {
	int** dist = problemData.dist;
	int nRounds = problemData.nRounds;
	int n = problemData.n;

	long cost = 0;

	for(int i = 0; i < n; i++) {
		for(int j = 0; j < nRounds-1; j++) {
			cost = cost + dist[solution[j][i].first][solution[j+1][i].first];
		}
	}
	return cost;
}

void setProblemData(int nTeams, int nRounds, int n, int** dist, int** opponents) {
	problemData.nTeams = nTeams;
	problemData.nRounds = nRounds;
	problemData.n = n;
	problemData.dist = dist;
	problemData.opponents = opponents;
}

void printSolution(pii** solution) {
	std::cout << "============================" << std::endl;
	for(int i = 0; i < problemData.nRounds; i++) {
		for(int j = 0; j < problemData.n; j++) {
			std::cout << "(" << solution[i][j].first << ", " << solution[i][j].second << ") ";
		}
		std::cout << std::endl;
	}
	std::cout << "============================" << std::endl;
}

void shuffle(int* myArray, size_t n) {
    for(int i = 0; i < n; i++) {
        myArray[i] = i;
    }
    if(n > 1) {
        size_t i;
        for (int i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = myArray[j];
            myArray[j] = myArray[i];
            myArray[i] = t;
        }
    }
}


// mutation
// 1 <= mutateRate <= nRounds
pii** mutation(pii** solution, int mutationRate) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;

	int* shRounds = new int[nRounds];
	shuffle(shRounds, nRounds);

	pii** newsolution = new pii*[problemData.nRounds];
	for(int i = 0; i < problemData.nRounds; i++) {
		newsolution[i] = new pii[problemData.n];
		for(int j = 0; j < problemData.n; j++) {
			//copying solution to newsolution
			newsolution[i][j] = solution[i][j];
		}
	}

	int r;
	int r1;
	int r2;
	pii temp;
	
	for(int i = 0; i < mutationRate; i++) {
		r = shRounds[i]; // random slot
		int* shGames = new int[n];
		shuffle(shGames, n);
		r1 = shGames[0];
		r2 = shGames[1];
		temp = newsolution[r][r1];
		newsolution[r][r1] = newsolution[r][r2];
		newsolution[r][r2] = temp;
		delete [] shGames;
	}

	delete [] shRounds;
	
	return newsolution;
}

// Feasibility test for constraint (3):
// Every umpire sees every team at least once at the team’s home
int feasibility3(pii** solution) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int* homeVisited = new int[nTeams];
	int venue;
	int contVenues = 0;
	int nViolations = 0;

	for(int u = 0; u < n; u++) {
		for(int i = 0; i < nTeams; i++) {
			homeVisited[i] = 0;
		}
		for(int i = 0; i < nRounds; i++) {
			venue = solution[i][u].first;
			if(homeVisited[venue] == 0) {
				homeVisited[venue] = 1;
				contVenues = contVenues + 1;
			}
		}
	}

	nViolations = nTeams*n - contVenues;
	
	//std::cout << "nViolations (c3) = " << nViolations << std::endl;

	delete[] homeVisited;

	return nViolations;
}

// Feasibility test for constraint (4):
// No umpire is in a home site more than once in any n - d1 consecutive slots
int feasibility4(pii** solution) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int* lastVisited = new int[nTeams];
	int venue;
	int nViolations = 0;

	for(int u = 0; u < n; u++) {
		for(int i = 0; i < nTeams; i++) {
			lastVisited[i] = -1*timewindow4;
		}
		for(int i = 0; i < nRounds; i++) {
			venue = solution[i][u].first;
			if(i - lastVisited[venue] < timewindow4) {
				nViolations = nViolations + 1;
			}
			lastVisited[venue] = i;
		}
	}

	delete[] lastVisited;

	//std::cout << "nViolations (c4) = " << nViolations << std::endl;

	return nViolations;
}

// Feasibility test for constraint (5):
// No umpire sees a team more than once in any n/2 - d2 consecutive slots.
int feasibility5(pii** solution) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int* lastSeen = new int[nTeams];
	int team1, team2;
	int nViolations = 0;

	for(int u = 0; u < n; u++) {
		for(int i = 0; i < nTeams; i++) {
			lastSeen[i] = -1*timewindow5;
		}
		for(int i = 0; i < nRounds; i++) {
			team1 = solution[i][u].first;
			team2 = solution[i][u].second;
			if((i - lastSeen[team1] < timewindow5)) {
				nViolations = nViolations + 1;
			}
			if((i - lastSeen[team2] < timewindow5)) {
				nViolations = nViolations + 1;
			}
			lastSeen[team1] = i;
			lastSeen[team2] = i;
		}
	}

	delete[] lastSeen;

	//std::cout << "nViolations (c5) = " << nViolations << std::endl;

	return nViolations;
}

int totalViolations(pii** solution) {
	int nViolations3 = feasibility3(solution);
	int nViolations4 = feasibility4(solution);
	int nViolations5 = feasibility5(solution);

	return nViolations3 + nViolations4 + nViolations5;
}

pii** tournamentSelection(std::vector< pii** > population) {
    pii** best = NULL;
    int r1, r2;
    int sizePopulation = population.size();
    pii** s1 = population[rand() % sizePopulation];
    pii** s2 = population[rand() % sizePopulation];

    if(totalViolations(s1) == 0 && totalViolations(s2) == 0) {
    	if(cost(s1) < cost(s2)) {
    		best = s1;
    	} else {
    		best = s2;
    	}
    } else if(totalViolations(s1) > 0 && totalViolations(s2) > 0) {
    	if(totalViolations(s1) < totalViolations(s2)) {
    		best = s1;
    	} else {
    		best = s2;
    	}
    } else {
    	if(totalViolations(s1) == 0) {
    		best = s1;
    	} else if(totalViolations(s2) == 0) {
    		best = s2;
    	}
    }

    return best;
}

void loadData() {
	std::ifstream file("data/umps8.txt");
	std::string str;
	std::vector<std::string> fileContents;
	
	while(std::getline(file, str)) {
		str = strip(str);
		if(str.find_first_not_of(' ') != std::string::npos) {
			std::string::iterator new_end = std::unique(str.begin(), str.end(), bothAreSpaces);
			str.erase(new_end, str.end());
			fileContents.push_back(str);
		}
	}

	int nTeams;
	int nRounds;
	int n;
	int** dist;
	int** opponents;

	std::stringstream convert(fileContents[0]);
	
	if ( !(convert >> nTeams) ) {
		std::cout << "Cannot convert string to int on nTeams" << std::endl;
	} else {
		nRounds = 2*nTeams - 2;
		n = nTeams/2;
		dist = new int*[nTeams];
		for(int i = 0; i < nTeams; i++) {
			dist[i] = new int[nTeams];
		}

		opponents = new int*[nRounds];
		for(int i = 0; i < nRounds; i++) {
			opponents[i] = new int[nTeams];
		}

		for(int i = 1; i < nTeams+1; i++) {
			std::stringstream input(fileContents[i]);
			for(int j = 0; j < nTeams; j++) {
	    		input >> dist[i-1][j];
	    	}
		}

		for(int i = nTeams+1; i < nTeams+1 + nRounds; i++) {
			std::stringstream input(fileContents[i]);
			for(int j = 0; j < nTeams; j++) {
	    		input >> opponents[i-nTeams-1][j];
	    	}
		}	
	}
	setProblemData(nTeams, nRounds, n, dist, opponents);
}

void crossover(pii** p1, pii** p2) {
	int nRounds = problemData.nRounds;
	int n = problemData.n;

	pii** offspring = new pii*[nRounds];
	for(int i = 0; i < nRounds; i++) {
		offspring[i] = new pii[n];
	}
	
	int crossPoint = rand() % nRounds;

	for(int i = 0; i < crossPoint; i++) {
		for(int j = 0; j < n; j++) {
			offspring[i][j] = p1[i][j];
		}
	}

	for(int i = crossPoint; i < nRounds; i++) {
		for(int j = 0; j < n; j++) {
			offspring[i][j] = p2[i][j];
		}
	}

	std::cout << crossPoint << std::endl;
	printSolution(p1);
	printSolution(p2);
	printSolution(offspring);
}

void run() {
	int nTeams = problemData.nTeams;
	int nRounds = problemData.nRounds;
	int n = problemData.n;
	int** dist = problemData.dist;
	int** opponents = problemData.opponents;

	pii** solution = new pii*[nRounds];
	for(int i = 0; i < nRounds; i++) {
		solution[i] = new pii[n];
	}

	int** games = new int*[nRounds];
	for(int i = 0; i < nRounds; i++) {
		games[i] = new int[nTeams];
	}

	for(int i = 0; i < nRounds; i++) {
		for(int j = 0; j < nTeams; j++) {
			games[i][j] = -1;
		}
	}
	
	int signal;
	int c;

	for(int i = 0; i < nRounds; i++) {
		c = 0;
		for(int j = 0; j < nTeams; j++) {
			if(games[i][j] == -1) {
				games[i][j] = c;
				if(opponents[i][j] > 0) {
					signal = 1;
					games[i][signal*opponents[i][j]-1] = c;
					solution[i][c].first = j;
					solution[i][c].second = signal*opponents[i][j]-1;
				} else {
					signal = -1;
					games[i][signal*opponents[i][j]-1] = c;
					solution[i][c].first = signal*opponents[i][j]-1;
					solution[i][c].second = j;
				}
				c++;
			}
		}
	}

	std::vector< pii** > population;
	const int penaltyRate = 100;

	for(int i = 0; i < 10; i++) {
		pii** newsolution = mutation(solution, nRounds-1);
		population.push_back(newsolution);
	}

	pii** p1 = tournamentSelection(population);
	pii** p2 = tournamentSelection(population);

	crossover(p1, p2);

	deleteMatrix(games, nRounds);
	deleteSolution(solution);
}

int main() {
	srand(1607);
	loadData();
	run();
	return 0;
}