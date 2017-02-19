#include <cmath>
#include "matrix.h"
#include <cstdlib>
#include <cstdio>

/*********************************************************/
/***************** matrix data structure *****************/
/*********************************************************/
MATRIX allocateMatrix(long row, long column){
	MATRIX M;
	long i;
    
	M.row = row;
	M.column = column;
	M.data = (int **) malloc(row * sizeof(int*));
	for (i = 0; i < row; i++)
		M.data[i] = (int *) malloc(column * sizeof(int));
	return M;
}
MATRIXD allocateMatrixD(long row, long column){
	MATRIXD M;
	long i;
    
	M.row = row;
	M.column = column;
	M.data = (double **) malloc(row * sizeof(double*));
	for (i = 0; i < row; i++)
		M.data[i] = (double *) malloc(column * sizeof(double));
	return M;
}
void freeMatrix(MATRIX M){
	long i;
	for (i = 0; i < M.row; i++)
		free(M.data[i]);
	free(M.data);
}
void freeMatrixD(MATRIXD M){
	long i;
	for (i = 0; i < M.row; i++)
		free(M.data[i]);
	free(M.data);
}
void initializeMatrix(MATRIX *M, int c){
	long i, j;
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			M->data[i][j] = c;
}
void initializeMatrixD(MATRIXD *M, double c){
	long i, j;
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			M->data[i][j] = c;
}
int maxMatrixEntry(MATRIX M){
	int maxEntry;
	long i, j;

	maxEntry = M.data[0][0];
	for (i = 0; i < M.row; i++)
		for (j = 0; j < M.column; j++)
			if (maxEntry < M.data[i][j])
				maxEntry = M.data[i][j];
	return maxEntry;
}
MATRIX readMatrix(const char *filename){
	MATRIX M;
	long r, c, i, j;

	FILE *id = fopen(filename,"r");
	if (id == NULL){
		printf("Error: File %s does not exist...\n",filename);
		exit(1);
	}
	fscanf(id,"%ld%ld",&r,&c);
	M = allocateMatrix(r,c);
	for (i = 0; i < r; i++)
		for (j = 0; j < c; j++)
			fscanf(id,"%d",&(M.data[i][j]));
	fclose(id);
	return M;
}
void writeMatrixIntoFile(MATRIX M, char *filename, int headerFlag){
    long i, j;
    FILE *id = fopen(filename,"w");
    
	if (headerFlag)
		fprintf(id,"%ld\t%ld\n",M.row,M.column);
	for (i = 0; i < M.row; i++){
		for (j = 0; j < M.column; j++)
			fprintf(id,"%d ",M.data[i][j]);
		fprintf(id,"\n");
	}
	fclose(id);
}

MATRIX convertMxArray2Matrix(const mxArray *xData){
	double *xValues = mxGetPr(xData);
	int row = mxGetM(xData);
	int col = mxGetN(xData);
	MATRIX res = allocateMatrix(row,col);
	int i, j, cnt = 0;
	
	for (j = 0; j < col; j++)
		for (i = 0; i < row; i++)
			res.data[i][j] = (int)xValues[cnt++];	
	
	return res;
}

MATRIXD convertMxArray2MatrixD(const mxArray *xData){
	double *xValues = mxGetPr(xData);
	int row = mxGetM(xData);
	int col = mxGetN(xData);
	MATRIXD res = allocateMatrixD(row,col);
	int i, j, cnt = 0;
	
	for (j = 0; j < col; j++)
		for (i = 0; i < row; i++)
			res.data[i][j] = (double)xValues[cnt++];	
	
	return res;
}

mxArray *convertMatrix2MxArray(MATRIX M){
	mxArray *res = mxCreateDoubleMatrix(M.row,M.column,mxREAL);
	double *xValues = mxGetPr(res);	
	int i, j, cnt = 0;
	
	for (j = 0; j < M.column; j++)
		for (i = 0; i < M.row; i++)
			xValues[cnt++] = M.data[i][j];

	return res;
}