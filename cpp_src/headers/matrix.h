#ifndef __MATRIX_H
#define __MATRIX_H
#include <mex.h>

/*********************************************************/
/***************** matrix data structure *****************/
/*********************************************************/
struct TMatrix{
	long row;
	long column;
	int **data;
};
typedef struct TMatrix  MATRIX;

struct TMatrixD{
	long row;
	long column;
	double **data;
};
typedef struct TMatrixD  MATRIXD;

MATRIX allocateMatrix(long row, long column);
MATRIXD allocateMatrixD(long row, long column);
void freeMatrix(MATRIX M);
void freeMatrixD(MATRIXD M);
void initializeMatrix(MATRIX *M, int c);
void initializeMatrixD(MATRIXD *M, double c);
int maxMatrixEntry(MATRIX M);
MATRIX readMatrix(const char *filename);
void writeMatrixIntoFile(MATRIX M, char *filename, int headerFlag);

MATRIX  convertMxArray2Matrix(const mxArray *xData);
MATRIXD convertMxArray2MatrixD(const mxArray *xData);
mxArray *convertMatrix2MxArray(MATRIX M);
#endif
