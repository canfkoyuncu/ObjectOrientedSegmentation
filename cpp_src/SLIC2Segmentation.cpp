#include "headers/matrix.h"
#include "headers/util.h"
#include <math.h>
#include <vector>
#include <cstdio>
#include <map>
#include <algorithm>    // std::find
#include <queue>        // std::priority_queue
#include <utility>      // std::pair, std::make_pair
#include "headers/Superpixel.h"

#define DOUBLE_MAX 1000000000.0
using namespace std;

double votingTh;

struct SlicPair{
   int label1, label2;
   pair<int, double> rightScore;
   pair<int, double> leftScore;
   pair<int, double> topScore;
   pair<int, double> bottomScore;
   int area1, area2;
};

struct PrimObj{
   int minr = -1, maxr = -1;
   int minc = -1, maxc = -1;
   vector<point> pixList;
};

void printfPair (SlicPair *sp) {
	mexPrintf ("%d %d R(%.2f - %d), L(%.2f - %d), T(%.2f - %d), B(%.2f - %d), %.2f\n", sp->label1, sp->label2, sp->rightScore.second, sp->rightScore.first, 
		sp->leftScore.second, sp->leftScore.first, sp->topScore.second, sp->topScore.first, sp->bottomScore.second, sp->bottomScore.first, votingTh);
}

void printfBoundingBoxSp (Superpixel *sp) {
	mexPrintf ("%d, (%d %d), (%d %d)\n", sp->label, sp->minr, sp->maxr, sp->minc, sp->maxc);
}

SlicPair makePair (int label1, int label2, double rightScore1, int rightId, double leftScore1, int leftId, double topScore1, int topId, double bottomScore1, int bottomId, int area1, int area2) {
	SlicPair sp;
	if (label1 < label2) {
		sp.label1 = label1;
		sp.label2 = label2;
		sp.area1 = area1;
		sp.area2 = area2;
	}
	else {
		sp.label1 = label2;
		sp.label2 = label1;
		sp.area1 = area2;
		sp.area2 = area1;
	}
	sp.rightScore  = make_pair (rightId, rightScore1);
	sp.leftScore   = make_pair (leftId, leftScore1);
	sp.topScore    = make_pair (topId, topScore1);
	sp.bottomScore = make_pair (bottomId, bottomScore1);
	return sp;
}

void updateSLIC (MATRIX *slic, map<int, Superpixel> &superpixels) {
	initializeMatrix (slic, 0);
	for (map<int, Superpixel>::iterator it=superpixels.begin(); it!=superpixels.end(); it++) {
		for (int i=0; i<it->second.pixList.size(); i++) {
			slic->data[it->second.pixList[i].i][it->second.pixList[i].j] = it->first;
		}
	}
}

bool operator<(const SlicPair& a, const SlicPair& b) {
	return (a.rightScore.second + a.leftScore.second + a.topScore.second + a.bottomScore.second) <= (b.rightScore.second + b.leftScore.second + b.topScore.second + b.bottomScore.second);
}

void eraseSuperpixel (map<int, Superpixel>& superpixels, int label) {
	map<int, Superpixel>::iterator it = superpixels.find(label);
	if (it != superpixels.end())
		superpixels.erase (it);
	for (it = superpixels.begin(); it != superpixels.end(); it++)
		it->second.removeNeighbor (label);
}

bool areSimilarSlicPairs (SlicPair *sp, bool twoSideFlag) {
	bool isRightSimilar 	= sp->rightScore.second > votingTh;
	bool isLeftSimilar 		= sp->leftScore.second > votingTh;
	bool isTopSimilar 		= sp->topScore.second > votingTh;
	bool isBottomSimilar 	= sp->bottomScore.second > votingTh;
	if (twoSideFlag)
		return (isLeftSimilar || isRightSimilar) && (isTopSimilar || isBottomSimilar);
	else 
		return (isLeftSimilar || isRightSimilar) || (isTopSimilar || isBottomSimilar);
}

double computePairScoreOnPixel (int i, int j, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, MATRIX *slic, enum side s, Superpixel *sp1, Superpixel *sp2) {
	int label;
	if (s == right) 	label = rightPrims->data[i][j];
	else if (s == left) label = leftPrims->data[i][j];
	else if (s == top)  label = topPrims->data[i][j];
	else 				label = bottomPrims->data[i][j];
	int minr = min(sp1->minr, sp2->minr);
	int maxr = max(sp1->maxr, sp2->maxr);
	int minc = min(sp1->minc, sp2->minc);
	int maxc = max(sp1->maxc, sp2->maxc);

	if (s == right) {
		while (isInRange(i, j, rightPrims->row, rightPrims->column) && rightPrims->data[i][j] == label)
			movePrim (i, j, s);
	}
	else if (s == left) {
		while (isInRange(i, j, rightPrims->row, rightPrims->column) && leftPrims->data[i][j] == label)
			movePrim (i, j, s);
	}
	else if (s == top) {
		while (isInRange(i, j, rightPrims->row, rightPrims->column) && topPrims->data[i][j] == label)
			movePrim (i, j, s);
	}
	else {
		while (isInRange(i, j, rightPrims->row, rightPrims->column) && bottomPrims->data[i][j] == label)
			movePrim (i, j, s);
	}

	if (isInRange(i, j, rightPrims->row, rightPrims->column)) {
		int prevLabel = slic->data[i][j];
		while (checkCondition (i, j, label, s, rightPrims, leftPrims, topPrims, bottomPrims, minr, maxr, minc, maxc, slic) ) {
			if ((prevLabel == sp1->label && slic->data[i][j] == sp2->label) || (prevLabel == sp2->label && slic->data[i][j] == sp1->label))
				return 1;
			else if (slic->data[i][j] == 0)
				return 0;
			prevLabel = slic->data[i][j];
			movePrim (i, j, s);
		}		
	}
	return 0;
}

void computePairScore (MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, int maxPrimLabel, MATRIX *slic, enum side s, Superpixel &sp1, Superpixel &sp2, double &pairScore, int &primId, PrimObj *primObjs) {
	if (!sp1.isNeighbor (sp2.label)) {
		primId = -1;
		pairScore = 0;
	}
	if (s == right && (sp1.rightVotingPrim.first == -1 || sp1.rightVotingPrim.first != sp2.rightVotingPrim.first)) {
		primId = -1;
		pairScore = 0;
	}
	else if (s == left && (sp1.leftVotingPrim.first == -1 || sp1.leftVotingPrim.first != sp2.leftVotingPrim.first)) {
		primId = -1;
		pairScore = 0;
	}
	else if (s == top && (sp1.topVotingPrim.first == -1 || sp1.topVotingPrim.first != sp2.topVotingPrim.first)) {
		primId = -1;
		pairScore = 0;
	}
	else if (s == bottom && (sp1.bottomVotingPrim.first == -1 || sp1.bottomVotingPrim.first != sp2.bottomVotingPrim.first)) {
		primId = -1;
		pairScore = 0;
	}
	else {
		int minr, maxr, minc, maxc;
		int votingRange;
		if (s == right) {
			primId = sp1.rightVotingPrim.first;
			votingRange = rightPrims->row + 1;
		}
		else if (s == left) {
			primId = sp1.leftVotingPrim.first;
			votingRange = rightPrims->row + 1;
		}
		else if (s == top) {
			primId = sp1.topVotingPrim.first;
			votingRange = rightPrims->column + 1;
		}
		else {
			primId = sp1.bottomVotingPrim.first;
			votingRange = rightPrims->column + 1;
		}

		int visited[votingRange];
		for (int j=0; j<votingRange; j++)
			visited[j] = 0;

		pairScore = 0;
		for (int k=0; k<primObjs[primId].pixList.size(); k++) {
			int i = primObjs[primId].pixList[k].i;
			int j = primObjs[primId].pixList[k].j;
			int ind;
			if (s == right || s == left)
				ind = i;
			else
				ind = j;
			if (visited[ind] == 0) {
				if (computePairScoreOnPixel (i, j, rightPrims, leftPrims, topPrims, bottomPrims, slic, s, &sp1, &sp2)) {
					pairScore++;
					visited[ind] = 1;
				}
			} 
		}

		if (s == right || s == left) {
            double r1 = sp1.maxr - sp1.minr + 1;
            double r2 = sp2.maxr - sp2.minr + 1;
            r1 = pairScore > 0 ? pairScore / r1 : 0;
            r2 = pairScore > 0 ? pairScore / r2 : 0;
            pairScore = (r1+r2) / 2;
		}
		else {
			double c1 = sp1.maxc - sp1.minc + 1;
            double c2 = sp2.maxc - sp2.minc + 1;
            c1 = pairScore > 0 ? pairScore / c1 : 0;
            c2 = pairScore > 0 ? pairScore / c2 : 0;
            pairScore = (c1+c2) / 2;
		}
	}
}

void updatePriorityQueue (map<int, Superpixel > &superpixels, priority_queue<SlicPair> &similarSlics, MATRIX *slic, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, int maxRightLabel, int maxLeftLabel, int maxTopLabel, int maxBottomLabel, int label1, int label2, PrimObj *rightBndBxs, PrimObj *leftBndBxs, PrimObj *topBndBxs, PrimObj *bottomBndBxs, bool twoSideFlag) {
	vector<SlicPair> temp;
	while (!similarSlics.empty()){
		SlicPair sp = similarSlics.top();
		similarSlics.pop();
		if (!(sp.label1 == label1 || sp.label2 == label1 || sp.label2 == label1 || sp.label2 == label2))
			temp.push_back (sp);
	}

	for (int ind=0; ind<temp.size(); ind++)
		similarSlics.push(temp[ind]);

	map<int, Superpixel>::iterator it1 = superpixels.find (label1);
	if (it1 != superpixels.end()) {
		for (int i=0; i<it1->second.adjList.size(); i++) {
			map<int, Superpixel>::iterator it2 = superpixels.find (it1->second.adjList[i]);
			if (it2 != superpixels.end()){
				double rightScore, leftScore, topScore, bottomScore;
				int rightId, leftId, topId, bottomId;
				computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxRightLabel, slic, right, it1->second, it2->second, rightScore, rightId, rightBndBxs);
				computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxLeftLabel, slic, left, it1->second, it2->second, leftScore, leftId, leftBndBxs);
				computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxTopLabel, slic, top, it1->second, it2->second, topScore, topId, topBndBxs);
				computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxBottomLabel, slic, bottom, it1->second, it2->second, bottomScore, bottomId, bottomBndBxs);
				
				SlicPair sp = makePair (it1->first, it2->first, rightScore, rightId, leftScore, leftId, topScore, topId, bottomScore, bottomId, it1->second.getArea(), it2->second.getArea());
				if (areSimilarSlicPairs (&sp, twoSideFlag))
					similarSlics.push(sp);
			}
		}
	}
}

void mergePair (map<int, Superpixel > &superpixels, MATRIX *slic, int label1, int label2, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, int maxRightLabel, int maxLeftLabel, int maxTopLabel, int maxBottomLabel, int maxDist) {
	map<int, Superpixel>::iterator it1 = superpixels.find(label1);
	map<int, Superpixel>::iterator it2 = superpixels.find(label2);

	if (it1 != superpixels.end() && it2 != superpixels.end()) {
		it1->second.isMerged = true;

		//update bounding box
		if(it1->second.minr > it2->second.minr)
			it1->second.minr = it2->second.minr;
		if(it1->second.minc > it2->second.minc)
			it1->second.minc = it2->second.minc;
		if(it1->second.maxr < it2->second.maxr)
			it1->second.maxr = it2->second.maxr;
		if(it1->second.maxc < it2->second.maxc)
			it1->second.maxc = it2->second.maxc;

		//update pixList
		for (int i=0; i<it2->second.pixList.size(); i++) {
			it1->second.pixList.push_back(it2->second.pixList[i]);
			slic->data[it2->second.pixList[i].i][it2->second.pixList[i].j] = it1->first;
		}

		//update neighbors
		it2->second.removeNeighbor(label1);
		for (int i=0; i<it2->second.adjList.size(); i++) {
			if (it2->second.adjList[i] != 0) {
				map<int, Superpixel>::iterator neighborOfIt2 = superpixels.find(it2->second.adjList[i]);
				if (neighborOfIt2 != superpixels.end()) {
					neighborOfIt2->second.removeNeighbor (label2);
					neighborOfIt2->second.addNeighbor (label1);
					it1->second.addNeighbor (neighborOfIt2->first);
				}
			}
		}

		//remove merged superpixel
		eraseSuperpixel (superpixels, label2);

		it1->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxRightLabel, right, maxDist);
		it1->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxLeftLabel, left, maxDist);
		it1->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxTopLabel, top, maxDist);
		it1->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxBottomLabel, bottom, maxDist);
	}
	else if (it2 != superpixels.end()) {
		superpixels[label1] = it2->second;
		eraseSuperpixel (superpixels, label2);
	}
}

void printPriorityQueue(priority_queue<SlicPair> similarSlics) {
	while (!similarSlics.empty()){
		SlicPair sp = similarSlics.top();
		similarSlics.pop();
		if (sp.area1 > 50 && sp.area2 > 50)
			printfPair(&sp);
	}
}

void printSuperpixels(map<int, Superpixel> superpixels) {
	for (map<int, Superpixel>::iterator it = superpixels.begin(); it!=superpixels.end(); it++){
		mexPrintf("%d -> ", it->first);
		for (int i=0; i<it->second.adjList.size(); i++)
			mexPrintf("%d, ", it->second.adjList[i]);
		mexPrintf("\n");
	}
}

void mergeBestPairs (MATRIX *slic, priority_queue<SlicPair> &similarSlics, map<int, Superpixel > &superpixels, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, int maxRightLabel, int maxLeftLabel, int maxTopLabel, int maxBottomLabel, int maxDist, PrimObj *rightBndBxs, PrimObj *leftBndBxs, PrimObj *topBndBxs, PrimObj *bottomBndBxs, bool twoSideFlag) {
	while (!similarSlics.empty()){
		SlicPair sp = similarSlics.top();
		similarSlics.pop();
		int label1 = sp.label1;
		int label2 = sp.label2;
		map<int, Superpixel>::iterator it1 = superpixels.find(label1);
		map<int, Superpixel>::iterator it2 = superpixels.find(label2);
		mergePair (superpixels, slic, label1, label2, rightPrims, leftPrims, topPrims, bottomPrims, maxRightLabel, maxLeftLabel, maxTopLabel, maxBottomLabel, maxDist);	//merge superpixels
		updatePriorityQueue (superpixels, similarSlics, slic, rightPrims, leftPrims, topPrims, bottomPrims, maxRightLabel, maxLeftLabel, maxTopLabel, maxBottomLabel, label1, label2, rightBndBxs, leftBndBxs, topBndBxs, bottomBndBxs, twoSideFlag);	//update priority queue	
	}
}

priority_queue<SlicPair> identifySimilarPairs (map<int, Superpixel> &superpixels, MATRIX *slic, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, int maxRightLabel, int maxLeftLabel, int maxTopLabel, int maxBottomLabel, PrimObj *rightBndBxs, PrimObj *leftBndBxs, PrimObj *topBndBxs, PrimObj *bottomBndBxs, bool twoSideFlag) {
	priority_queue<SlicPair> similarPairs;

	int maxLabel = superpixels.rbegin()->first;
	int **visited = new int*[maxLabel+1];
	for (int i=0; i<maxLabel+1; i++)
		visited[i] = new int[maxLabel+1];

	for (int i=0; i<maxLabel+1; i++)
		for (int j=0; j<maxLabel+1; j++)
			visited[i][j] = 0;
	
    for (map<int, Superpixel>::iterator it1 = superpixels.begin(); it1 != superpixels.end(); it1++) {
    	for (int i = 0; i < it1->second.adjList.size(); i++) {
    		map<int, Superpixel>::iterator it2 = superpixels.find(it1->second.adjList[i]);

    		if (visited[it1->first][it2->first] == 0) {
    			visited[it1->first][it2->first] = 1;
    			visited[it2->first][it1->first] = 1;
	    		double rightScore, leftScore, topScore, bottomScore;
	    		int rightId, leftId, topId, bottomId;
	    		computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxRightLabel, slic, right, it1->second, it2->second, rightScore, rightId, rightBndBxs);
	    		computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxLeftLabel, slic, left, it1->second, it2->second, leftScore, leftId, leftBndBxs);
	    		computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxTopLabel, slic, top, it1->second, it2->second, topScore, topId, topBndBxs);
	    		computePairScore (rightPrims, leftPrims, topPrims, bottomPrims, maxBottomLabel, slic, bottom, it1->second, it2->second, bottomScore, bottomId, bottomBndBxs);

				SlicPair sp = makePair (it1->first, it2->first, rightScore, rightId, leftScore, leftId, topScore, topId, bottomScore, bottomId, it1->second.getArea(), it2->second.getArea());
				if (areSimilarSlicPairs (&sp, twoSideFlag))
					similarPairs.push(sp);

				int label1 = -584, label2 = 602;
				if ((sp.label1 == label1 && sp.label2 == label2) || (sp.label1 == label2 && sp.label2 == label1))
					printfPair(&sp);
    		}
        }
    }

    for (int i=0; i<maxLabel+1; i++)
    	delete [] visited[i];
    delete [] visited;

    return similarPairs;
}

map<int, Superpixel > identifySuperpixels (MATRIX *slic, int connectivity, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, int maxRightLabel, int maxLeftLabel, int maxTopLabel, int maxBottomLabel, int maxDist) {
	map<int, Superpixel > superpixels;
	int i, j, i2, j2, k, label;
	for (i=0; i<slic->row; i++) {
		for (j=0; j<slic->column; j++) {
			label = slic->data[i][j];
			if (label) {
				map<int, Superpixel>::iterator it = superpixels.find(label);
				point p; p.i = i; p.j = j;
				if (it == superpixels.end()) {
					Superpixel sp;
					sp.label = label;
					sp.minr = i;	sp.maxr = i;
					sp.minc = j;	sp.maxc = j;
					sp.pixList.push_back(p);
					if (connectivity == 4) {
						for (k=0; k<d4; k++) {
							i2 = i + neighbors4[k][0];
							j2 = j + neighbors4[k][1];
							if (i2 >= 0 && j2 >= 0 && i2 < slic->row && j2 < slic->column && label != slic->data[i2][j2] && find(sp.adjList.begin(), sp.adjList.end(), slic->data[i2][j2]) == sp.adjList.end())
									sp.addNeighbor (slic->data[i2][j2]);
						}
					}
					else if(connectivity == 8) {
						for (k=0; k<d8; k++) {
							i2 = i + neighbors8[k][0];
							j2 = j + neighbors8[k][1];
							if (i2 >= 0 && j2 >= 0 && i2 < slic->row && j2 < slic->column && label != slic->data[i2][j2] && find(sp.adjList.begin(), sp.adjList.end(), slic->data[i2][j2]) == sp.adjList.end())
									sp.addNeighbor (slic->data[i2][j2]);
						}
					}
					superpixels[label] = sp;
				}
				else {
					it->second.pixList.push_back(p);
					if (it->second.minr > i)	it->second.minr = i;
					if (it->second.minc > j)	it->second.minc = j;
					if (it->second.maxr < i)	it->second.maxr = i;
					if (it->second.maxc < j)	it->second.maxc = j;
					if (connectivity == 4) {
						for (k=0; k<d4; k++) {
							i2 = i + neighbors4[k][0];
							j2 = j + neighbors4[k][1];
							if (i2 >= 0 && j2 >= 0 && i2 < slic->row && j2 < slic->column && label != slic->data[i2][j2] && find(it->second.adjList.begin(), it->second.adjList.end(), slic->data[i2][j2]) == it->second.adjList.end())
								it->second.addNeighbor (slic->data[i2][j2]);
						}
					}
					else if(connectivity == 8) {
						for (k=0; k<d8; k++) {
							i2 = i + neighbors8[k][0];
							j2 = j + neighbors8[k][1];
							if (i2 >= 0 && j2 >= 0 && i2 < slic->row && j2 < slic->column && label != slic->data[i2][j2] && find(it->second.adjList.begin(), it->second.adjList.end(), slic->data[i2][j2]) == it->second.adjList.end())
								it->second.addNeighbor (slic->data[i2][j2]);
						}
					}
				}
			}
		}
	}

	for (map<int, Superpixel>::iterator it=superpixels.begin(); it!=superpixels.end(); it++) {
		it->second.setMeanPoint();
		
		it->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxRightLabel, right, maxDist);
		it->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxLeftLabel, left, maxDist);
		it->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxTopLabel, top, maxDist);
		it->second.defineVotingPrims (rightPrims, leftPrims, topPrims, bottomPrims, slic, maxBottomLabel, bottom, maxDist);

		int label1 = -1;	
		if (it->first == label1)
			mexPrintf ("%d R(%.4f - %d), L(%.4f - %d), T(%.4f - %d), B(%.4f - %d)\n", it->first, 	it->second.rightVotingPrim.second, it->second.rightVotingPrim.first, 
																									it->second.leftVotingPrim.second, it->second.leftVotingPrim.first,
																									it->second.topVotingPrim.second, it->second.topVotingPrim.first,
																									it->second.bottomVotingPrim.second, it->second.bottomVotingPrim.first);
	}
	return superpixels;
}

PrimObj* identifyBndBx (MATRIX *prims, int maxPrimLabel) {
	PrimObj *primObjs = new PrimObj[maxPrimLabel+1];
	for (int i=0; i<prims->row; i++) {
		for (int j=0; j<prims->column; j++) {
			int label = prims->data[i][j];
			if (label) {
				if (primObjs[label].minr == -1 || primObjs[label].minr > i)
					primObjs[label].minr = i;
				if (primObjs[label].maxr == -1 || primObjs[label].maxr < i)
					primObjs[label].maxr = i;
				if (primObjs[label].minc == -1 || primObjs[label].minc > j)
					primObjs[label].minc = j;
				if (primObjs[label].maxc == -1 || primObjs[label].maxc < j)
					primObjs[label].maxc = j;
				point p;
				p.i = i; p.j = j;
				primObjs[label].pixList.push_back(p);
			}
		}
	}

	return primObjs;
}

void printPairInfo (priority_queue<SlicPair> pairs, int label1, int label2) {
	vector<SlicPair> temp;
	while (!pairs.empty()){
		SlicPair sp = pairs.top();
		pairs.pop();
		if ((sp.label1 == label1 && sp.label2 == label2) || (sp.label1 == label2 && sp.label2 == label1))
			printfPair (&sp);
	}
}

void eliminateSmallSuperpixels (map<int, Superpixel> &superpixels, int areaTh, MATRIX *cells) {
	initializeMatrix (cells, 0);
	int label = 1;
	for (map<int, Superpixel>::iterator it=superpixels.begin(); it!=superpixels.end(); it++) {
		if (it->second.getArea() > areaTh) {
			for (int i = 0; i<it->second.pixList.size(); i++)
				cells->data[it->second.pixList[i].i][it->second.pixList[i].j] = label;
			label++;
		}
	}
}

void mexFunction (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
	MATRIX slic 		= convertMxArray2Matrix (prhs[0]);
	MATRIX rightPrims 	= convertMxArray2Matrix (prhs[1]);
	MATRIX leftPrims 	= convertMxArray2Matrix (prhs[2]);
	MATRIX topPrims 	= convertMxArray2Matrix (prhs[3]);
	MATRIX bottomPrims 	= convertMxArray2Matrix (prhs[4]);
    votingTh			= mxGetScalar (prhs[5]);
	int maxMaxDist 		= mxGetScalar (prhs[6]);
	int connectivity 	= mxGetScalar (prhs[7]);
	int twoSideFlag 	= mxGetScalar (prhs[8]);
	
	int maxRightLabel 	= maxMatrixEntry (rightPrims);
	int maxLeftLabel 	= maxMatrixEntry (leftPrims);
	int maxTopLabel 	= maxMatrixEntry (topPrims);
	int maxBottomLabel 	= maxMatrixEntry (bottomPrims);

	PrimObj* rightBndBxs 	= identifyBndBx (&rightPrims, maxRightLabel);
	PrimObj* leftBndBxs 	= identifyBndBx (&leftPrims, maxLeftLabel);
	PrimObj* topBndBxs 		= identifyBndBx (&topPrims, maxTopLabel);
	PrimObj* bottomBndBxs 	= identifyBndBx (&bottomPrims, maxBottomLabel);
	
	map<int, Superpixel> superpixels;
	for (int maxDist = 1; maxDist <= maxMaxDist; maxDist++) {
		superpixels = identifySuperpixels (&slic, connectivity, &rightPrims, &leftPrims, &topPrims, &bottomPrims, maxRightLabel, maxLeftLabel, maxTopLabel, maxBottomLabel, maxDist);
		priority_queue<SlicPair> similarSlics = identifySimilarPairs (superpixels, &slic, &rightPrims,  &leftPrims,  &topPrims,  &bottomPrims, maxRightLabel, maxLeftLabel, maxTopLabel, maxBottomLabel, rightBndBxs, leftBndBxs, topBndBxs, bottomBndBxs, twoSideFlag);
		mergeBestPairs (&slic, similarSlics, superpixels, &rightPrims, &leftPrims, &topPrims, &bottomPrims, maxRightLabel, maxLeftLabel, maxTopLabel, maxBottomLabel, maxDist, rightBndBxs, leftBndBxs, topBndBxs, bottomBndBxs, twoSideFlag);
	}
	plhs[0] = convertMatrix2MxArray (slic);
	

	delete [] rightBndBxs;
	delete [] leftBndBxs;
	delete [] topBndBxs;
	delete [] bottomBndBxs;
	freeMatrix (slic);
	freeMatrix (rightPrims);
	freeMatrix (leftPrims);
	freeMatrix (topPrims);
	freeMatrix (bottomPrims);
}
