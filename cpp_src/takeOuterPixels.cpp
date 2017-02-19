#include "headers/matrix.h"
#include "headers/util.h"
#include <math.h>
#include <vector>
#include <queue>        // std::priority_queue
#include <algorithm>
#include <map>

using namespace std;

enum side {
  top, 
  bottom, 
  right, 
  left 
};

void takeOneSideBoundary (MATRIX *prims, enum side sideFlag, int thickness) {
	int i, j, k, i2, j2;
	MATRIX processed = allocateMatrix (prims->row, prims->column);

	for (i=0; i<prims->row; i++) {
		for (j=0; j<prims->column; j++) {
			if (prims->data[i][j] > 0) {
				bool isBorder = false;
				for (k=0; k<=thickness; k++) {
					if (sideFlag == right) {
						i2 = i;
						j2 = j + k;
					}
					else if (sideFlag == left) {
						i2 = i;
						j2 = j - k;
					}
					else if (sideFlag == top) {
						i2 = i - k;
						j2 = j;
					}
					else {
						i2 = i + k;
						j2 = j;
					}
					if (sideFlag == right && j2 >= prims->column) {
						isBorder = true;
						break;
					}
					if (sideFlag == left && j2 < 0) {
						isBorder = true;
						break;
					}
					if (sideFlag == top && i2 < 0) {
						isBorder = true;
						break;
					}
					if (sideFlag == bottom && i2 >= prims->row) {
						isBorder = true;
						break;
					}
					if (prims->data[i2][j2] != prims->data[i][j]) {
						isBorder = true;
						break;
					}
				}
				if (isBorder)
					processed.data[i][j] = prims->data[i][j];
				else
					processed.data[i][j] = 0;
			}
			else
				processed.data[i][j] = 0;
		}
	}

	for (i=0; i<prims->row; i++)
		for (j=0; j<prims->column; j++)
			prims->data[i][j] = processed.data[i][j];
	freeMatrix (processed);
}

/*
void takeOneSideBoundary (MATRIX *prims, enum side sideFlag, int thickness) {
	int i, j, k, i2, j2;
	MATRIX processed = allocateMatrix (prims->row, prims->column);

	if (sideFlag == right) {
		for (i=0; i<prims->row; i++) {
			for (j=0; j<prims->column; j++) {
				if (prims->data[i][j] > 0) {
					bool isBorder = false;
					for (k=0; k<=thickness; k++) {
						i2 = i;
						j2 = j + k;
						if (j2 >= prims->column) {
							isBorder = true;
							break;
						}
						else if (prims->data[i2][j2] != prims->data[i][j]) {
							isBorder = true;
							break;
						}
					}
					if (isBorder)
						processed.data[i][j] = prims->data[i][j];
					else
						processed.data[i][j] = 0;
				}
				else
					processed.data[i][j] = 0;
			}
		}
	}
	else if (sideFlag == left){
		for (i=0; i<prims->row; i++) {
			for (j=0; j<prims->column; j++) {
				if (prims->data[i][j] > 0) {
					bool isBorder = false;
					for (k=0; k<=thickness; k++) {
						i2 = i;
						j2 = j - k;
						if (i2 < 0 || i2 >= prims->row || j2 < 0 || j2 >= prims->column) {
							isBorder = true;
							break;
						}
						else if (prims->data[i2][j2] != prims->data[i][j]) {
							isBorder = true;
							break;
						}
					}
					if (isBorder)
						processed.data[i][j] = prims->data[i][j];
					else
						processed.data[i][j] = 0;
				}
				else
					processed.data[i][j] = 0;
			}
		}

	}
	else if (sideFlag == top){
		for (j=0; j<prims->column; j++) {
			for (i=0; i<prims->row; i++) {
 				if (prims->data[i][j] > 0) {
					bool isBorder = false;
					for (k=0; k<=thickness; k++) {
						i2 = i - k;
						j2 = j;
						if (i2 < 0 || i2 >= prims->row || j2 < 0 || j2 >= prims->column) {
							isBorder = true;
							break;
						}
						else if (prims->data[i2][j2] != prims->data[i][j]) {
							isBorder = true;
							break;
						}		
					}
					if (isBorder)
						processed.data[i][j] = prims->data[i][j];
					else
						processed.data[i][j] = 0;
				}
				else
					processed.data[i][j] = 0;
			}
		}
	}
	else if (sideFlag == bottom) {
		for (j=0; j<prims->column; j++) {
			for (i=0; i<prims->row; i++) {
				if (prims->data[i][j] > 0) {
					bool isBorder = false;
					for (k=0; k<=thickness; k++) {
						i2 = i + k;
						j2 = j;
						if (i2 < 0 || i2 >= prims->row || j2 < 0 || j2 >= prims->column) {
							isBorder = true;
							break;
						}
						else if (prims->data[i2][j2] != prims->data[i][j]) {
							isBorder = true;
							break;
						}		
					}
					if (isBorder)
						processed.data[i][j] = prims->data[i][j];
					else
						processed.data[i][j] = 0;
				}
				else
					processed.data[i][j] = 0;
			}
		}
	}

	for (i=0; i<prims->row; i++)
		for (j=0; j<prims->column; j++)
			prims->data[i][j] = processed.data[i][j];
	freeMatrix (processed);
}*/

void mexFunction (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
	MATRIX rightPrims 		= convertMxArray2Matrix (prhs[0]);
	MATRIX leftPrims 		= convertMxArray2Matrix (prhs[1]);
	MATRIX topPrims 		= convertMxArray2Matrix (prhs[2]);
	MATRIX bottomPrims 		= convertMxArray2Matrix (prhs[3]);
	int thickness			= mxGetScalar (prhs[4]);
	
	takeOneSideBoundary (&rightPrims, right, thickness);
	takeOneSideBoundary (&leftPrims, left, thickness);
	takeOneSideBoundary (&topPrims, top, thickness);
	takeOneSideBoundary (&bottomPrims, bottom, thickness);
	
	
    plhs[0] = convertMatrix2MxArray (rightPrims);
	plhs[1] = convertMatrix2MxArray (leftPrims);
	plhs[2] = convertMatrix2MxArray (topPrims);
	plhs[3] = convertMatrix2MxArray (bottomPrims);
	
	freeMatrix (rightPrims);
	freeMatrix (leftPrims);
	freeMatrix (topPrims);
	freeMatrix (bottomPrims);
}