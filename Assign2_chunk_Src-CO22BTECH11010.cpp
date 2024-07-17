#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <pthread.h>

using namespace std;

#define MAX_ROWS 1300
#define MAX_COLS 1300
#define affinity 1 // can change its value to zero, by doing so entire sceduling will be handled by OS

int N = 0;
int K = 0;
vector<vector<int>> matrixA;
vector<vector<int>> matrixC;
int C = 0;
int BT = 0;
int b;

struct ThreadData
{
    int startRow;
    int endRow;
    int threadId; // Added to count the treads so that they can be assigned to specific core
};

void matmul(ThreadData data)
{
    // Set t affinity
    if (affinity == 1)
    {
        if (data.threadId < BT)
        {
            // Print the thread ID and the core it's assigned to
            cout << data.threadId << ": " << (data.threadId / b) % C << endl;

            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            // Distribute threads uniformly among cores
            CPU_SET((data.threadId / b) % C, &cpuset);

            if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
            {
                cerr << "Error setting thread affinity" << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    // Matrix multiplication for the assigned rows
    for (int i = data.startRow; i <= data.endRow; i++)
    {
        for (int j = 0; j < N; j++)
        {
            matrixC[i][j] = 0;

            for (int k = 0; k < N; k++)
            {
                matrixC[i][j] += matrixA[i][k] * matrixA[k][j];
            }
        }
    }
}

int main()
{
    // Measure the starting time
    chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

    // Open the input file
    ifstream input("matrix.txt");

    if (!input.is_open())
    {
        cerr << "Error opening input file" << endl;
        exit(EXIT_FAILURE);
    }

    // Read matrix dimensions and other parameters
    input >> N >> K >> C >> BT;

    // Validate input
    if (C == 0)
    {
        cerr << "Error: C cannot be zero." << endl;
        exit(EXIT_FAILURE);
    }

    // Calculate rows per thread and threads per core
    int rowsperthread = N / K;
    b = K / C;

    // Resize matrices
    matrixA.resize(N, vector<int>(N));
    matrixC.resize(N, vector<int>(N));

    // Read matrix A from the file
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            input >> matrixA[i][j];
        }
    }

    // Close the input file
    input.close();

    // Create thread objects and thread data structures
    vector<thread> threads;
    vector<ThreadData> thread_data;
    int currentRow = 0;

    // Divide the work among threads
    for (int i = 0; i < K - 1; i++)
    {
        thread_data.push_back({currentRow, currentRow + rowsperthread - 1, i});
        threads.emplace_back(matmul, thread_data[i]);
        currentRow = thread_data[i].endRow + 1;
    }

    // Handle the last thread
    thread_data.push_back({currentRow, N - 1, K - 1});
    threads.emplace_back(matmul, thread_data[K - 1]);

    // Wait for all threads to finish
    for (int i = 0; i < K; ++i)
    {
        threads[i].join();
    }

    // Measure the end time
    chrono::steady_clock::time_point end_time = chrono::steady_clock::now();

    // Calculate total time taken
    double total_time = chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count();

    // Open the output file
    ofstream output("out1.txt");

    // Print the total time taken by the code
    output << "\ntime taken by code: " << total_time << " seconds\n";

    // Write the result matrix to the output file
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            output << matrixC[i][j] << " ";
        }
        output << endl;
    }

    // Close the output file
    output.close();

    // Print the total time taken by the code to the console
    cout << "\ntime taken by code: " << total_time << " seconds\n";

    return 0;
}
