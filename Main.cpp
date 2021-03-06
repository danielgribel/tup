#include "MinAssignment.h"
#include "Solution.h"
#include "Heap.h"
#include "HashTable.h"
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

const double MAX_LONG = std::numeric_limits<long>::max();

ProblemData problemData;
string inputFile;
const int penaltyRate = 100000;
const int d4 = 7; // d4 = n - d1
const int d5 = 3; // d5 = floor(n/2) - d2 (rounding n/2 down)

Solution* segmentsMatching(Solution* solution);

Solution* localSearch(Solution* solution);

Solution* localImprovement(Solution* solution) {
	solution = segmentsMatching(solution);
	solution = localSearch(solution);
}

bool isNum(char c) {
    return std::string("-0123456789 ").find(c) != std::string::npos;
}

std::string strip(const std::string &s) {
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

void deleteMatrix(double** matrix, int n) {
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


/*void printSolution(pii** solution) {
	std::cout << "============================" << std::endl;
	for(int i = 0; i < problemData.n; i++) {
		for(int j = 0; j < problemData.nRounds; j++) {
			std::cout << "(" << solution[j][i].first+1 << ", " << solution[j][i].second+1 << ")\t";
		}
		std::cout << std::endl;
	}
	std::cout << "============================" << std::endl;
}*/

void printSolution(pii** solution) {
	std::cout << "============================" << std::endl;
	for(int i = 0; i < problemData.n; i++) {
		for(int j = 0; j < problemData.nRounds; j++) {
			std::cout << solution[j][i].first+1 << " ";
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

void fill(int* myArray, size_t n) {
    for(int i = 0; i < n; i++) {
        myArray[i] = i;
    }
}

// mutation
// 1 <= mutateRate <= nRounds
pii** mutation(pii** solution, const int mutationRate, const int q) {
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
	
	/*for(int i = 0; i < mutationRate; i++) {
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

	for(int i = 0; i < mutationRate; i++) {
		r = shRounds[i]; // random slot
		int* shGames = new int[n];
		shuffle(shGames, n);
		for(int j = 0; j < n; j++) {
			newsolution[r][j] = solution[r][shGames[j]];
		}
		delete [] shGames;
	}*/

	int j, g;
	/*for(int i = 0; i < mutationRate; i++) {
		r = shRounds[i]; // random slot
		int* shGames = new int[n];
		shuffle(shGames, n);
		for(int j = 0; j < q; j++) {
			newsolution[r][j] = solution[r][shGames[j]];
			newsolution[r][shGames[j]] = solution[r][j];
		}
		delete [] shGames;
	}*/

	for(int i = 0; i < mutationRate; i++) {
		r = shRounds[i]; // random slot
		int* shGames = new int[n];
		shuffle(shGames, n);
		for(int j1 = 0; j1 < q; j1++) {
			j = shGames[j1];
			g = shGames[n-j1-1];
			temp = newsolution[r][j];
			newsolution[r][j] = newsolution[r][g];
			newsolution[r][g] = temp;
		}
		delete [] shGames;
	}

	delete [] shRounds;
	
	return newsolution;
}

Solution* construct(pii** baseScheduling) {
	int nRounds = problemData.nRounds;
	int nTeams = problemData.nTeams;
	int n = problemData.n;
	int** dist = problemData.dist;

	pii** greedyScheduling = new pii*[nRounds];
	for(int i = 0; i < nRounds; i++) {
		greedyScheduling[i] = new pii[n];
	}

	for(int i = 0; i < n; i++) {
		greedyScheduling[0][i] = baseScheduling[0][i];
	}

	int* visited = new int[n];
	int u;
	long max;

	for(int i = 1; i < nRounds; i++) {
		int* shGames = new int[n];
		shuffle(shGames, n);
		for(int j = 0; j < n; j++) {
			visited[j] = 0;
		}
		for(int j = 0; j < n; j++) {
			max = 99999999;
			u = shGames[j];
			if(dist[greedyScheduling[i-1][u].first][baseScheduling[i][j].first] < max && visited[j] == 0) {
				visited[j] = 1;
				greedyScheduling[i][u].first = baseScheduling[i][j].first;
				greedyScheduling[i][u].second = baseScheduling[i][j].second;
			}
		}
		delete [] shGames;
	}
	delete [] visited;
	Solution* greedySolution = new Solution(greedyScheduling, problemData);
	greedySolution = segmentsMatching(greedySolution);

	return greedySolution;
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
	std::ifstream file(inputFile.c_str());
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
	setProblemData(nTeams, nRounds, n, dist, opponents, d4, d5, penaltyRate);
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

	deleteMatrix(matchingMatrix, n);
	
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

	//std::cout << "crossPoint = " << crossPoint << std::endl;

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

	/*std::vector<long> matching = match(offspring, crossPoint);

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

	deleteSolution(offspring);*/

	return offspring;
}

std::vector<Solution*> selectSurvivors(HeapPdi* costHeap, std::vector<Solution*> population, int sizePopulation) {

    HashTable* table = new HashTable();
    HeapPdi* heapInd = new HeapPdi();
    HeapPdi* heapClones = new HeapPdi();

    const int maxPopulation = population.size();
    std::vector<Solution*> newPopulation;
    int* discarded = new int[maxPopulation];
    int id;

    if(population.size() < sizePopulation) {
    	sizePopulation = population.size();
    }

    for(int i = 0; i < maxPopulation; i++) {
        Solution* solution = population[i];
        Item * anItem = new Item;
        (*anItem).cost = solution->getCorrectedCost();
        (*anItem).nbViolations3 = solution->getNbViolations3();
        (*anItem).nbViolations4 = solution->getNbViolations4();
        (*anItem).nbViolations5 = solution->getNbViolations5();
        (*anItem).next = NULL;

        if(table->existItem(anItem)) {
            heapClones->push_max(solution->getCorrectedCost(), i);
        } else {
            table->insertItem(anItem);
            heapInd->push_max(solution->getCorrectedCost(), i);
        }
        discarded[i] = 0;
    }
    int j = 0;
    
    while((j < (maxPopulation-sizePopulation)) && (heapClones->getHeap().size() > 0)) {
        id = heapClones->front_max().second;
        heapClones->pop_max();
        discarded[id] = 1;
        j++;
    }

    while(j < (maxPopulation-sizePopulation)) {
        id = heapInd->front_max().second;
        heapInd->pop_max();
        discarded[id] = 1;
        j++;
    }

    HeapPdi* costHeapAux = new HeapPdi();
    int l = 0;

    for(int i = 0; i < maxPopulation; i++) {
        if(discarded[i] == 0) {
            newPopulation.push_back(population[i]);
            costHeapAux->push_min(population[i]->getCorrectedCost(), l);
            l++;
        } else {
            delete population[i];
        }
    }
    costHeap->setHeap(costHeapAux->getHeap());

    delete heapInd;
    delete heapClones;
    delete costHeapAux;
    delete [] discarded;
    delete table;

    return newPopulation;
}

std::vector<Solution*> getRandomPopulation(std::vector<Solution*> elitePopulation, int numNew) {
	std::vector<Solution*> randomPopulation;
	int j = 0;
	int r;
	for(int i = 0; i < numNew; i++) {
		//r = rand() % elitePopulation.size();
		pii** mutated = mutation(elitePopulation[j]->getScheduling(), problemData.nRounds/2, problemData.n/2);
		Solution* mutatedSolution = new Solution(mutated, problemData);
		mutatedSolution = localSearch(mutatedSolution);
		randomPopulation.push_back(mutatedSolution);
		j++;
		if(j >= 1.0*elitePopulation.size()) {
			j = 0;
		}
	}
	return randomPopulation;
}

std::vector<Solution*> diversifyPopulation(
	HeapPdi* costHeap, std::vector<Solution*> population, const int numKeep, const int numNew) {

    std::vector<Solution*> newPopulation;
    long cost;
    int id;

    HeapPdi* costHeapAux = new HeapPdi();

    for(int i = 0; i < numKeep; i++) {
        cost = costHeap->front_min().first;
        id = costHeap->front_min().second;
        costHeapAux->push_min(cost, i);
        costHeap->pop_min();
        newPopulation.push_back(population[id]);
    }

    std::vector<Solution*> randomIndividuals = getRandomPopulation(newPopulation, numNew);

    for(int i = 0; i < randomIndividuals.size(); i++) {
        newPopulation.push_back(randomIndividuals[i]);
        cost = randomIndividuals[i]->getCorrectedCost();
        costHeapAux->push_min(cost, numKeep+i);
    }

    costHeap->setHeap(costHeapAux->getHeap());

    delete costHeapAux;

    return newPopulation;
}

Solution* segmentsMatching(Solution* solution) {
	int nRounds = problemData.nRounds;
	int n = problemData.n;
	int crossPoint;
	int* shRounds = new int[nRounds-1];
	bool improving = true;
	
	while(improving) {
		shuffle(shRounds, nRounds-1);
		improving = false;
		for(int i = 0; i < nRounds-1; i++) {
			crossPoint = shRounds[i]+1;
			std::vector<long> matching = match(solution->getScheduling(), crossPoint);

			pii** offspringMatching = new pii*[nRounds];
			for(int i = 0; i < nRounds; i++) {
				offspringMatching[i] = new pii[n];
			}

			for(int i = 0; i < crossPoint; i++) {
				for(int j = 0; j < n; j++) {
					offspringMatching[i][j] = solution->getScheduling()[i][j];
				}
			}

			int s;
			for(int i = crossPoint; i < nRounds; i++) {
				for(int j = 0; j < n; j++) {
					s = matching[j];
					offspringMatching[i][j] = solution->getScheduling()[i][s];
				}
			}

			Solution* newSolution = new Solution(offspringMatching, problemData);
			
			if(newSolution->getCorrectedCost() < solution->getCorrectedCost()) {
				delete solution;
				solution = NULL;
				solution = newSolution;
				improving = true;
			} else {
				delete newSolution;
			}
		}	
	}
	
	delete [] shRounds;

	return solution;
}

Solution* localSearch(Solution* solution) {
	int nRounds = problemData.nRounds;
	int n = problemData.n;
	int* shRounds = new int[nRounds];
	bool improving = true;
	int r, i, j;
	pii temp;

	int totalViolationsDelta;
	int totalViolationsDeltaI;
	int totalViolationsDeltaJ;
	long costDelta;
	long costDeltaI;
	long costDeltaJ;
	long newCorrectedCost;

	while(improving) {
		shuffle(shRounds, nRounds);
		improving = false;
		for(int i1 = 0; i1 < nRounds; i1++) {
			r = shRounds[i1];
			int* shGames = new int[n];
			shuffle(shGames, n);
			for(int i1 = 0; i1 < n; i1++) {
				i = shGames[i1];
				for(int j1 = i1+1; j1 < n; j1++) {
					j = shGames[j1];
					totalViolationsDeltaI = solution->umpireConst3Delta(i, j, r) +
											solution->umpireConst4Delta(i, j, r) +
											solution->umpireConst5Delta(i, j, r);
					
					totalViolationsDeltaJ = solution->umpireConst3Delta(j, i, r) +
											solution->umpireConst4Delta(j, i, r) +
											solution->umpireConst5Delta(j, i, r);

					totalViolationsDelta = totalViolationsDeltaI + totalViolationsDeltaJ;

					costDeltaI = solution->umpireCostDelta(i, j, r);
					costDeltaJ = solution->umpireCostDelta(j, i, r);
					costDelta = costDeltaI + costDeltaJ;

					newCorrectedCost = problemData.penaltyRate*(solution->getTotalViolations() + totalViolationsDelta) +
										(solution->getCost() + costDelta);

					//int nv = solution->getNbViolations5() + solution->umpireConst5Delta(i, j, r) + solution->umpireConst5Delta(j, i, r);

					if(newCorrectedCost < solution->getCorrectedCost()) {
						solution->swap(i, j, r);
						improving = true;
						//std::cout << "assert? = " << (solution->getNbViolations5() == nv) << std::endl;
					}
				}
			}
			delete [] shGames;
		}	
	}
	
	delete [] shRounds;

	return solution;
}

void run() {
	int nTeams = problemData.nTeams;
	int nRounds = problemData.nRounds;
	int n = problemData.n;
	int** dist = problemData.dist;
	int** opponents = problemData.opponents;

	const int maxPopulation = 1200;
	const int sizePopulation = 200;
	const int maxIt = 50000;
	const int itNoImp = 20000;
	const int initPopSize = 500;
	const int itDiv = 2000;
	const int numKeep = sizePopulation;
	const int numNew = 2*sizePopulation;
	
	HeapPdi* costHeap = new HeapPdi();

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
	solution = localImprovement(solution);
	Solution* bestSolution = solution;
	long bestCost = solution->getCorrectedCost();
	Solution* currSolution;
	long ccost;
	int lastImprovement = 0;
    int lastDiv = 0;
	int it = 0;

	std::cout << solution->getCost() << " " << solution->getTotalViolations() << std::endl;

	for(int i = 0; i < initPopSize; i++) {
		pii** newScheduling = mutation(bestSolution->getScheduling(), nRounds/2, n/2);
		Solution* newSolution = new Solution(newScheduling, problemData);
		newSolution = localSearch(newSolution);
		population.push_back(newSolution);
		costHeap->push_min(newSolution->getCorrectedCost(), i);
		if(newSolution->getCorrectedCost() < bestSolution->getCorrectedCost()) {
			bestSolution = newSolution;
		}
		std::cout << i << ") = " << newSolution->getTotalViolations() << std::endl;
	}

	while((it-lastImprovement < itNoImp) && (it < maxIt)) {
		pii** p1 = tournamentSelection(population);
		pii** p2 = tournamentSelection(population);
		
		pii** offspring = crossover(p1, p2);
		Solution* offspringSolution = new Solution(offspring, problemData);

		/*if(offspringSolution->getTotalViolations() == 0) {
			std::cout << "p1\n";
			printSolution(p1);
			std::cout << "p2\n";
			printSolution(p2);
			std::cout << "off\n";
			printSolution(offspringSolution->getScheduling());
		}*/

		pii** mutated = mutation(offspring, 1, 1);
		Solution* mutatedSolution = new Solution(mutated, problemData);

		offspringSolution = localImprovement(offspringSolution);

		population.push_back(offspringSolution);
		costHeap->push_min(offspringSolution->getCorrectedCost(), population.size() - 1);

		population.push_back(mutatedSolution);
		costHeap->push_min(mutatedSolution->getCorrectedCost(), population.size() - 1);

		if(offspringSolution->getCorrectedCost() < mutatedSolution->getCorrectedCost()) {
			ccost = offspringSolution->getCorrectedCost();
			currSolution = offspringSolution;
		} else {
			ccost = mutatedSolution->getCorrectedCost();
			currSolution = mutatedSolution;
		}
		if(ccost < bestSolution->getCorrectedCost()) {
			bestSolution = currSolution;
			lastImprovement = it;
		}

		if(population.size() > maxPopulation) {
            population = selectSurvivors(costHeap, population, sizePopulation);
        }

        if( ((it-lastImprovement) >= itDiv) && ((it-lastDiv) >= itDiv) ) {
            lastDiv = it;
            population = diversifyPopulation(costHeap, population, numKeep, numNew);
            if(costHeap->front_min().first < bestSolution->getCorrectedCost()) {
                lastImprovement = it;
                // should copy. may be getting a deleted solution
                bestSolution = population[costHeap->front_min().second];
            }
        }

        std::cout << "f(" << it << ") = "
					<< bestSolution->getCorrectedCost() << " "
					<< bestSolution->getCost() << " "
					<< bestSolution->getTotalViolations() << " "
					<< lastImprovement << std::endl;
		it++;
	}

	printSolution(bestSolution->getScheduling());

	delete costHeap;
	deleteMatrix(games, nRounds);
	for(int i = 0; i < population.size(); i++) {
		delete population[i];
	}
}

int main(int argc, char** argv) {
	srand(0);
	
	/*Get the arguments passed by the user*/
	inputFile = argv[1];

	loadData();
	
	clock_t begin = clock();
	run();
	double elapsedSecs = double(clock() - begin) / CLOCKS_PER_SEC;
	printf("t = %.5f\n", elapsedSecs);    
	
	return 0;
}