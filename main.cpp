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

int cost(pii** solution) {
	int** dist = problemData.dist;
	int nRounds = problemData.nRounds;
	int n = problemData.n;

	int cost = 0;

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
	for(int i = 0; i < problemData.nRounds; i++) {
		for(int j = 0; j < problemData.n; j++) {
			std::cout << "(" << solution[i][j].first << ", " << solution[i][j].second << ") ";
		}
		std::cout << std::endl;
	}
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


// mutation: 1 <= mutateRate <= nRounds
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

	setProblemData(nTeams, nRounds, n, dist, opponents);	
	pii** newsolution = mutation(solution, 5);
	deleteMatrix(games, nRounds);
}

int main() {
	loadData();
	return 0;
}