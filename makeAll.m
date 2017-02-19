disp('Adding matlab_src and mex_src folders into Matlab path...');
addpath(genpath('matlab_src/'));
addpath('mex_src/');

disp('Compiling mex files...');
cd mex_src/
mex ../cpp_src/headers/util.cpp -c
mex ../cpp_src/headers/matrix.cpp -c

mex ../cpp_src/slicmex.c -output slicmex
mex util.o matrix.o ../cpp_src/SLIC2Segmentation.cpp -output objOrientSegm
mex util.o matrix.o ../cpp_src/takeOuterPixels.cpp -output takeOuterPixels
mex util.o matrix.o ../cpp_src/relabelImage.cpp -output relabelImage
mex util.o matrix.o ../cpp_src/modeFilter2D.cpp -output modeFilter2D

delete('matrix.o');
delete('util.o');
cd ..