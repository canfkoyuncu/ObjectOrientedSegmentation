
#include "headers/matrix.h"

/*int d = 4;
long neighbors[][2] = {        {0,1},
                        {-1,0}       , {1,0},
                               {0,-1} };*/
int d = 8;
long neighbors[][2] = { {-1,1}, {0,1}, {1,1},
                        {-1,0}       , {1,0},
                        {-1,-1}, {0,-1}, {1,-1} };

MATRIX connectivity (MATRIX *image){
    MATRIX visited = allocateMatrix(image->row,image->column);
    MATRIX regions = allocateMatrix(image->row,image->column);
    long *queueX = (long *)malloc(image->row * image->column * sizeof(long));
    long *queueY = (long *)malloc(image->row * image->column * sizeof(long));
    int i, j, k, x, y, x2, y2;
    long queueStart, queueEnd;
    long label = 0;

    initializeMatrix(&visited,0);
    initializeMatrix(&regions,0);
    for (i = 0; i < image->row; i++) {
        for (j = 0; j < image->column; j++){
            if (image->data[i][j] == 0)
                continue;
            if (visited.data[i][j] == 1)
                continue;

            label++;
            queueX[0] = i;
            queueY[0] = j;
            queueStart = 0;
            queueEnd = 1;
            visited.data[i][j] = 1;
                
            while (queueEnd > queueStart){
                x = queueX[queueStart];
                y = queueY[queueStart];
                regions.data[x][y] = label;
                for (k=0; k<d; k++) {
                    x2 = x + neighbors[k][0];
                    y2 = y + neighbors[k][1];
                    if (x2 >= 0 && y2 >= 0 && x2 < image->row && y2 < image->column && visited.data[x2][y2] != 1) {
                        if (image->data[x2][y2] == image->data[x][y]) {
                            visited.data[x2][y2] = 1;
                            queueX[queueEnd] = x2;
                            queueY[queueEnd] = y2;
                            queueEnd++;
                        }
                    }
                }
                queueStart++;
            }
        }
    }
    free(queueX);
    free(queueY);
    freeMatrix(visited);
    return regions;
}
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
    MATRIX im    = convertMxArray2Matrix(prhs[0]);
    MATRIX result   = connectivity (&im);
    plhs[0] = convertMatrix2MxArray(result);

    freeMatrix(im);
    freeMatrix(result);
}