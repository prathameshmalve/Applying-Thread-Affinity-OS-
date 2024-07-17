##Assignment 2:

##description:This C++ program To evaluate the efficiency of a parallel matrix squaring algorithm by studying the impact
	      of assigning individual threads to CPU cores. THe two parallelization techniques used are 'Chunk' and 'Mixed'
	  

## Instructions for Execution

This README provides instructions on how to compile and execute the submitted C++ program for efficient matrix squaring.

## Files included:
	Assgn2_Src-CO22BTECH11010_chunk.cpp: C++ source code for matrix multiplication with multithreading.
	Assgn2_Src-CO22BTECH11010_mixed.cpp: C++ source code for matrix multiplication with multithreading.
	matrix.txt: Input file containing matrices and other parameters.

## compilation
-Open a terminal
-Navigate to the directory containing the source code.
-complie the code using command :
			g++ Assgn2_Src-CO22BTECH11010_chunk.cpp ....for chunk method
			g++ Assgn2_Src-CO22BTECH11010_mixed.cpp ....for mixed method
			
-execute the code using command:
	./a.out for both the source codes
	
## Input files:

	Input files will be given in a .txt format which will contain values of N,K,C,BT and the matrix of size N.
	we will read the contents of the file using fscanf and store them in variables

Output:
The program will generate an output file (out1.txt) containing the result of matrix multiplication. Additionally, the execution time will be printed to the console.
