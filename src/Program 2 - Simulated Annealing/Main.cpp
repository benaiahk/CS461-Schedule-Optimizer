#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "State.h"
using namespace std;

// e ^ -[ (change in score) / T ]
double accProb(int oldScore, int newScore, double T) { 
	double exponent = -((oldScore - newScore) / T);
	return exp(exponent);
}

double randNum() { return ((double)rand() / (double)(RAND_MAX)); }

void simulatedAnnealing(double T, double min, double r) {
	srand(time(NULL));

	double maxT = T;
	double minT = min;
	double rate = r;
	int attempts = 0;
	int successfulChanges = 0;
	int localBest = 0;

	State initState;
	initState.debugConditions();
	initState.fillRandomCourses();

	while (maxT > minT) {
		bool changesMade = false;
		for (int i = 0; i < 4001; i++) {
			if (i == 4000) {
				cout << "4000 iterations made, lowering the temp... (" << (maxT * rate) << ")" << endl;
				break;
			}

			attempts++;

			State nextState(initState.courseVec);
			nextState.makeRandomChange();

			int fOld = initState.getFitness();
			int fNew = nextState.getFitness();

			// Successful change if entered.
			if (fOld < fNew || randNum() <= accProb(fOld, fNew, maxT)) {
				// Output and set new best.
				if (localBest < fNew) {
					nextState.foutputCourses("bestSolution.txt");
					localBest = fNew;
				}

				initState = nextState;
				initState.foutputCourses("Solution.txt");

				// Increment successful changes and reset pass counter.
				successfulChanges++;
				attempts = 0;
			}

			if (successfulChanges == 400) {
				cout << "400 successful changes, resetting counter and lowering temp... (" << (maxT * rate) << ")" << endl;
				successfulChanges = 0;
				maxT *= rate;
			}

			// 4000 attempts without change
			if (attempts == 4000) break;
		}

		// Complete pass.
		if (attempts == 4000) {
			cout << "Complete pass. Exiting." << endl;
			break;
		}

		maxT *= rate;
	}
}

int main() {
	int trials = 1;

	int trialCount = trials;
	while (trialCount != 0) {
		// T value, T min, T decrease rate
		simulatedAnnealing(1.0, 0.01, 0.9);
		trialCount--;
		cout << endl;
	}
	
	cout << trials << " Trial(s) ran." << endl;
	system("pause");
}