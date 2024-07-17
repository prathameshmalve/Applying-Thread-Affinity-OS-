#include <iostream>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <thread>
#include <ctime>

using namespace std;

#define MAX_ROWS 1300
#define MAX_COLS 1300
#define affinity 1 // can change its value to zero, by doing so entire sceduling will be handled by OS

int N = 0;
int K = 0;
int BT = 0;
int C = 0;
int b;
int **matrixA;
int **matrixC;

// Data structure for thread-specific information
struct ThreadData
{
    int *rows; // points to the array having index of rows of matrixA  to be assigned to particular thread
    int rowSize;
    int id; // here using this as a counter for performing experiment 1
};

// Function to perform matrix multiplication for a set of rows
void matmul(ThreadData *data)
{
    int *idx = data->rows;

    // Set affinity
    if (affinity == 1)
    {
        if (data->id < BT && b != 0)
        {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);

            // Distribute threads uniformly among available cores
            CPU_SET((data->id / b) % C, &cpuset);

            if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
            {
                cerr << "Error setting thread affinity" << endl;
                exit(EXIT_FAILURE);
            }
            // Uncomment the line below if you want to print the core each thread is assigned to
            // cout << "Thread " << data->id << " is on core " << (data->id / b) % C << endl;
        }
    }

    // Matrix multiplication for the assigned rows
    for (int i = 0; i < data->rowSize; i++)
    {
        for (int j = 0; j < N; ++j)
        {
            matrixC[idx[i]][j] = 0;

            for (int k = 0; k < N; ++k)
            {
                matrixC[idx[i]][j] += matrixA[idx[i]][k] * matrixA[k][j];
            }
        }
    }

    // Clean up dynamically allocated memory
    delete[] data->rows;
    delete[] data;
}

int main()
{
    // Measure the starting time
    clock_t start_time, end_time;
    start_time = clock();

    ifstream input("matrix.txt");

    if (!input.is_open())
    {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Read matrix dimensions and other parameters
    input >> N >> K >> C >> BT;
    int rowsperthread = N / K;
    b = K / C;

    // Allocate memory for matrices
    matrixA = new int *[N];
    matrixC = new int *[N];

    for (int i = 0; i < N; ++i)
    {
        matrixA[i] = new int[N];
        matrixC[i] = new int[N];
        for (int j = 0; j < N; ++j)
        {
            input >> matrixA[i][j];
        }
    }

    input.close();

    vector<thread> threads;

    // Divide the work among threads
    for (int i = 0; i < K; i++)
    {
        ThreadData *data = new ThreadData;
        int *row = new int[N];

        int count = 0;
        for (int j = 0; (j * K + i) < N; j++)
        {
            row[j] = j * K + i;
            count++;
        }

        // Set thread-specific data
        data->rows = row;
        data->rowSize = count;
        data->id = i;

        // Create and start a new thread
        threads.emplace_back(matmul, data);
    }

    // Wait for all threads to finish
    for (int i = 0; i < K; ++i)
    {
        threads[i].join();
    }

    // Open the output file
    ofstream output("out2.txt");

    // Write the result matrix to the output file
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            output << matrixC[i][j] << " ";
        }

        output << endl;
    }

    // Clean up dynamically allocated memory
    for (int i = 0; i < N; i++)
    {
        delete[] matrixA[i];
        delete[] matrixC[i];
    }
    delete[] matrixA;
    delete[] matrixC;

    // Measure the end time
    end_time = clock();

    // Calculate total time taken
    double total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Open the output file again to append the time taken
    output.open("out2.txt", ios::app);
    output << "\ntime taken by code: " << total_time << " seconds\n";

    // Close the output file
    output.close();

    // Print the total time taken by the code to the console
    cout << "time taken by code: " << total_time << " seconds\n";

    return 0;
}
