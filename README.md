# Parallelisation-of-DES-Algorithm
This project was done as part of the course CSE4001 - Parallel &amp; Distributed Computing by:
1. Talatala Rahul Reddy (19BCE1778)
2. Bandaru Kedaarnath		(19BCE1370)<br>

## About this algorithm and implementation
Cryptography is related to the study of converting plain text into unreadable text or cipher text and vice versa. To put in simple terms, it is a process of storing and transmitting data in a pre-defined form so that only those for whom it is intended can read and process it. Cryptography doesn't just protect data from theft Most cryptographic algorithms work more efficiently when implemented on multiple processors in parallel than in software that runs on a single processor.<br>
Taking into account the current algorithm, they are implemented serially and on a single processor. All of the existing algorithms, especially DES, are more complex and the execution time is relatively slow and requires a much longer execution time. Alternatively, cryptographic algorithms can be implemented in software that runs on multiple processors. In this project we have implemented the DES algorithm in parallel using tools like OpenMP.<br>

We have parallelised the DES algorithm by splitting the plain text into substrings of length '8' and as DES algorithm is a block cipher text algorithm we have utilised this oppurtunity to parallelize the algorithm. We have choosen to split the plain text into substrings of length '8' because we converted the characters to ASCII representation in 8-bit binary form so 8 characters * 8 bits = 64 bits which is the ideal input to the DES algorithm. Also we tried to parallelize the S-Box Permuation using nested parallelism but failed to achive good results due to thread overhead.<br>


## About the repository
1. header_files - Contains header files about different permutation tables and conversions used in the following implementations
2. serial_des.cpp - Serial Implementation of DES algorithm
3. parallel_des_v1.cpp - Parallel Implementation of DES algorithm
4. parallel_des_v2.cpp - Nested Parallelism Implementation of DES algorithm

## Commands to run the programs
### 1. serial_des.cpp:
To compile the program: ''' g++ -o sdes -fopenmp serial_des.cpp '''
To run the program: ''' ./sdes plaintext_file.txt '''
### 2. parallel_des_v1.cpp:
To compile the program: ''' g++ -o pdes_v1 -fopenmp parallel_des_v1.cpp '''
To run the program: ''' ./pdes_v1 plaintext_file.txt '''
### 3. parallel_des_v2.cpp:
To compile the program: ''' g++ -o pdes_v2 -fopenmp parallel_des_v2.cpp '''
To run the program: ''' ./pdes_v2 plaintext_file.txt '''
