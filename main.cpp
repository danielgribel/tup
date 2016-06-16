#include "min_cost_assignment.h"
#include "Solution.h"
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

ProblemData problemData;

const double MAX_LONG = std::numeric_limits<long>::max();

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

void setProblemData(int nTeams, int nRounds, int n, int** dist, int** opponents, 
	int timewindow4, int timewindow5, long penaltyRate) {
	
	problemData.nTeams = nTeams;
	problemData.nRounds = nRounds;
	problemData.n = n;
	problemData.dist = dist;
	problemData.opponents = opponents;
	problemData.timewindow4 = timewindow4;
	problemData.timewindow5 = timewindow5;
	problemData.penaltyRate = penaltyRate;
}

void printSolution(pii** solution) {
	std::cout << "============================" << std::endl;
	for(int i = 0; i < problemData.nRounds; i++) {
		std::cout << "[" << i << "] ";
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

int uFeasibility3(pii** solution, int u, int s, int crossPoint) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int* homeVisited = new int[nTeams];
	int venue;
	int contVenues = 0;
	int nViolations = 0;

	for(int i = 0; i < nTeams; i++) {
		homeVisited[i] = 0;
	}
	for(int i = 0; i < crossPoint; i++) {
		venue = solution[i][u].first;
		if(homeVisited[venue] == 0) {
			homeVisited[venue] = 1;
			contVenues = contVenues + 1;
		}
	}
	for(int i = crossPoint; i < nRounds; i++) {
		venue = solution[i][s].first;
		if(homeVisited[venue] == 0) {
			homeVisited[venue] = 1;
			contVenues = contVenues + 1;
		}
	}

	nViolations = nTeams - contVenues;
	
	delete[] homeVisited;

	return nViolations;
}

int uFeasibility4(pii** solution, int u, int s, int crossPoint) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int* lastVisited = new int[nTeams];
	int venue;
	int nViolations = 0;

	for(int i = 0; i < nTeams; i++) {
		lastVisited[i] = -1*problemData.timewindow4;
	}
	for(int i = 0; i < crossPoint; i++) {
		venue = solution[i][u].first;
		if(i - lastVisited[venue] < problemData.timewindow4) {
			nViolations = nViolations + 1;
		}
		lastVisited[venue] = i;
	}
	for(int i = crossPoint; i < nRounds; i++) {
		venue = solution[i][s].first;
		if(i - lastVisited[venue] < problemData.timewindow4) {
			nViolations = nViolations + 1;
		}
		lastVisited[venue] = i;
	}

	delete[] lastVisited;

	return nViolations;
}

int uFeasibility5(pii** solution, int u, int s, int crossPoint) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int* lastSeen = new int[nTeams];
	int team1, team2;
	int nViolations = 0;

	for(int i = 0; i < nTeams; i++) {
		lastSeen[i] = -1*problemData.timewindow5;
	}
	for(int i = 0; i < crossPoint; i++) {
		team1 = solution[i][u].first;
		team2 = solution[i][u].second;
		if((i - lastSeen[team1] < problemData.timewindow5)) {
			nViolations = nViolations + 1;
		}
		if((i - lastSeen[team2] < problemData.timewindow5)) {
			nViolations = nViolations + 1;
		}
		lastSeen[team1] = i;
		lastSeen[team2] = i;
	}
	for(int i = crossPoint; i < nRounds; i++) {
		team1 = solution[i][s].first;
		team2 = solution[i][s].second;
		if((i - lastSeen[team1] < problemData.timewindow5)) {
			nViolations = nViolations + 1;
		}
		if((i - lastSeen[team2] < problemData.timewindow5)) {
			nViolations = nViolations + 1;
		}
		lastSeen[team1] = i;
		lastSeen[team2] = i;
	}

	delete[] lastSeen;

	return nViolations;
}

pii** tournamentSelection(std::vector< Solution* > population) {
    pii** best = NULL;
    int r1, r2;
    int sizePopulation = population.size();
    Solution* s1 = population[rand() % sizePopulation];
    Solution* s2 = population[rand() % sizePopulation];

    if(s1->getTotalViolations() == 0 && s2->getTotalViolations() == 0) {
    	if(s1->getCost() < s1->getCost()) {
    		best = s1->getScheduling();
    	} else {
    		best = s2->getScheduling();
    	}
    } else if(s1->getTotalViolations() > 0 && s2->getTotalViolations() > 0) {
    	if(s1->getTotalViolations() < s2->getTotalViolations()) {
    		best = s1->getScheduling();
    	} else {
    		best = s2->getScheduling();
    	}
    } else {
    	if(s1->getTotalViolations() == 0) {
    		best = s1->getScheduling();
    	} else if(s2->getTotalViolations() == 0) {
    		best = s2->getScheduling();
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
	setProblemData(nTeams, nRounds, n, dist, opponents, 4, 2, 100000);
}

std::vector<long> match(pii** solution, int crossPoint) {
	const int n = problemData.n;
	const int nRounds = problemData.nRounds;
	int** dist = problemData.dist;
	double** matchingMatrix = new double*[n];
	for(int i = 0; i < n; i++) {
		matchingMatrix[i] = new double[n];
	}

	//std::cout << "crossPoint = " << crossPoint << std::endl;

	for(int u = 0; u < n; u++) {
		for(int s = 0; s < n; s++) {
			matchingMatrix[u][s] = 1.0*(dist[solution[crossPoint-1][u].first][solution[crossPoint][s].first]
									+ problemData.penaltyRate*(	  uFeasibility3(solution, u, s, crossPoint) 
													+ uFeasibility4(solution, u, s, crossPoint)
													+ uFeasibility5(solution, u, s, crossPoint)
									));
			//std::cout << matchingMatrix[u][s] << " ";
			/*std::cout << "edgeCost(" << u << ", " << s << ") = " << dist[solution[crossPoint-1][u].first][solution[crossPoint][s].first] << std::endl;
			std::cout << "uFeasibility3(" << u << ", " << s << ") = " << uFeasibility3(solution, u, s, crossPoint) << std::endl;
			std::cout << "uFeasibility4(" << u << ", " << s << ") = " << uFeasibility4(solution, u, s, crossPoint) << std::endl;
			std::cout << "uFeasibility5(" << u << ", " << s << ") = " << uFeasibility5(solution, u, s, crossPoint) << std::endl;
			std::cout << "=====================================" << std::endl;*/
		}
		//std::cout << std::endl;
	}

	std::vector<long> matching = minAssignment(matchingMatrix, n);
	
	return matching;
}

pii** crossover(pii** p1, pii** p2) {
	int nRounds = problemData.nRounds;
	int n = problemData.n;

	pii** offspring = new pii*[nRounds];
	for(int i = 0; i < nRounds; i++) {
		offspring[i] = new pii[n];
	}
	
	int crossPoint = rand() % (nRounds-1) + 1;

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

	std::vector<long> matching = match(offspring, crossPoint);

	//for(int i = 0; i < n; i++) {
	//	std::cout << matching[i] << " ";
	//} std::cout << std::endl;

	pii** offspringMatching = new pii*[nRounds];
	for(int i = 0; i < nRounds; i++) {
		offspringMatching[i] = new pii[n];
	}

	for(int i = 0; i < crossPoint; i++) {
		for(int j = 0; j < n; j++) {
			offspringMatching[i][j] = p1[i][j];
		}
	}

	int s;
	for(int i = crossPoint; i < nRounds; i++) {
		for(int j = 0; j < n; j++) {
			s = matching[j];
			offspringMatching[i][j] = p2[i][s];
		}
	}

	deleteSolution(offspring);

	return offspringMatching;
}

void run() {
	int nTeams = problemData.nTeams;
	int nRounds = problemData.nRounds;
	int n = problemData.n;
	int** dist = problemData.dist;
	int** opponents = problemData.opponents;

	pii** scheduling = new pii*[nRounds];
	for(int i = 0; i < nRounds; i++) {
		scheduling[i] = new pii[n];
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
					scheduling[i][c].first = j;
					scheduling[i][c].second = signal*opponents[i][j]-1;
				} else {
					signal = -1;
					games[i][signal*opponents[i][j]-1] = c;
					scheduling[i][c].first = signal*opponents[i][j]-1;
					scheduling[i][c].second = j;
				}
				c++;
			}
		}
	}

	std::vector<Solution*> population;
	Solution* solution = new Solution(scheduling, problemData);
	Solution* bestSolution = solution;
	long bestCost = solution->getCorrectedCost();
	Solution* currSolution;
	long ccost;
	int it = 0;
	std::cout << "f(" << it << ") = " << bestSolution->getCorrectedCost() << std::endl;

	for(int i = 0; i < 100; i++) {
		pii** newScheduling = mutation(solution->getScheduling(), nRounds-1);
		Solution* newSolution = new Solution(newScheduling, problemData);
		population.push_back(newSolution);
		if(newSolution->getCorrectedCost() < bestSolution->getCorrectedCost()) {
			bestSolution = newSolution;
		}
	}

	while(it < 10000) {
		pii** p1 = tournamentSelection(population);
		pii** p2 = tournamentSelection(population);
		pii** offspring = crossover(p1, p2);	
		pii** mutated = mutation(offspring, 2);

		Solution* offspringSolution = new Solution(offspring, problemData);
		Solution* mutatedSolution = new Solution(mutated, problemData);

		population.push_back(offspringSolution);
		population.push_back(mutatedSolution);

		if(offspringSolution->getCorrectedCost() < mutatedSolution->getCorrectedCost()) {
			ccost = offspringSolution->getCorrectedCost();
			currSolution = offspringSolution;
		} else {
			ccost = mutatedSolution->getCorrectedCost();
			currSolution = mutatedSolution;
		}
		if(ccost < bestSolution->getCorrectedCost()) {
			bestSolution = currSolution;
		}

		it++;
		std::cout << "f(" << it << ") = "
					<< bestSolution->getCorrectedCost() << " "
					<< bestSolution->getCost() << " "
					<< bestSolution->getTotalViolations() << std::endl;
	}
	
	deleteMatrix(games, nRounds);
	deleteSolution(scheduling);
}

int main() {
	srand(0);
	loadData();
	
	clock_t begin = clock();
	run();
	double elapsedSecs = double(clock() - begin) / CLOCKS_PER_SEC;
	printf("t = %.5f\n", elapsedSecs);    
	
	return 0;
}