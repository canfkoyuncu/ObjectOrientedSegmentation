disp('Adding matlab_src and mex_src folders into Matlab path...');
addpath(genpath('matlab_src/'));
addpath('cpp_src/');

disp('Compiling mex files...');
cd cpp_src/
mex ../cpp_src/headers/util.cpp -c
mex ../cpp_src/headers/matrix.cpp -c

mex ../cpp_src/slicmex.c -output slicmex
mex util.o matrix.o ../cpp_src/SLIC2Segmentation.cpp -output objOrientSegm

delete('matrix.o');
delete('util.o');
cd ..