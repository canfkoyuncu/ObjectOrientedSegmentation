#include "headers/matrix.h"
#include "headers/util.h"
#include <math.h>
#include <vector>
#include <algorithm>    // std::find

using namespace std;

void modeFilter (MATRIX *map, int radius, int ignoreBg) {
	MATRIX res = allocateMatrix(map->row, map->column);
	int maxLabel = maxMatrixEntry(*map)+1;
	for (int i=0; i<map->row; i++) {
		for (int j=0; j<map->column; j++) {
			if (ignoreBg && map->data[i][j] == 0)
				res.data[i][j] = 0;
			else {
				int values[maxLabel];
				for (int k=0; k<maxLabel; k++)
					values[k] = 0;

				for (int i2=-(radius-1); i2<=(radius-1); i2++) {
					for (int j2=-(radius-1); j2<=(radius-1); j2++) {
						if ((i2*i2 + j2*j2) <= radius*radius) {
							int r = i + i2;
							int c = j + j2;
							if (r >= 0 && c >= 0 && r < map->row && c < map->column)
								values[map->data[r][c]]++;
						}
					}
				}
				int	maxInd = 0;
				int maxVal = values[0];
				for (int k=1; k<maxLabel; k++)
					if (values[k] > maxVal) {
						maxVal = values[k];
						maxInd = k;
					}

				res.data[i][j] = maxInd;
			}
		}
	}

	for (int i=0; i<res.row; i++)
		for (int j=0; j<res.column; j++)
			map->data[i][j] = res.data[i][j];

	freeMatrix (res);
}

void mexFunction (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
	MATRIX map 		= convertMxArray2Matrix (prhs[0]);
	int radius 		= mxGetScalar (prhs[1]);
	int ignoreBg 	= mxGetScalar (prhs[2]);

    modeFilter (&map, radius, ignoreBg);
    plhs[0] = convertMatrix2MxArray (map);

	freeMatrix (map);
}