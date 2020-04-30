#include <iostream>
#include <fstream>
#include <omp.h>
#include <iomanip>

using namespace std;

double serial_sum(double numbers[], int total_numbers)
{
    double nsum = 0.0;
    for (int i = 0; i < total_numbers; i++)
    {
        nsum += numbers[i];
    }
    return nsum;
}


double parallel_sum(double numbers[], int total_numbers, 
    int thread_count)
{
    double sum = 0.0;
#pragma omp parallel num_threads(thread_count)
    {
        int rank = omp_get_thread_num();
        int num_parts = total_numbers / thread_count;
        int myfirst_i = num_parts * rank;
        int mylast_i;
        if (rank == thread_count - 1)
            mylast_i = total_numbers;
        else mylast_i = myfirst_i + num_parts;
        
        double my_sum = 0.0;
        for (int i = myfirst_i; i < mylast_i; i++)
        {
            my_sum += numbers[i];
        }
#pragma omp critical
        {
            sum += my_sum;
        }
    }
    return sum;
}

double parallel_sumv2(double numbers[], int total_numbers, int thread_count)
{
    double sum = 0.0;
#pragma omp parallel num_threads(thread_count)
    {
        double my_sum = 0.0;
#pragma omp for
        for (int i = 0; i < total_numbers; i++)
        {
            my_sum += numbers[i];
        }
#pragma omp critical
        {
            sum += my_sum;
        }
    }
    return sum;
}

double parallel_sumv3(double numbers[], int total_numbers, int thread_count)
{
    double nsum = 0.0;

#pragma omp parallel for num_threads(thread_count) reduction(+:nsum)
    for (int i = 0; i < total_numbers; i++)
    {
        nsum += numbers[i];
    }
    return nsum;
}

double parallel_sumv4(double numbers[], int total_numbers, int thread_count)
{
    double nsum = 0.0;
#pragma omp parallel for num_threads(thread_count) reduction(+:nsum)
    for (int i = 0; i < total_numbers; i++)
    {
        nsum += numbers[i];
    }
    return nsum;
}
int main()
{
    int total[26] = { 0 };
    int thread_count = 4;
    double* numbers = new double[total[25]];
    double sum;
    ifstream myfile("gen.txt");
    if (!myfile)
    {
        cout << "Can not open file" << endl;
        return 0;
    }
    char letter;
    while (myfile.get(letter))
    {
        if (isalpha(letter))
        {           
            letter = toupper(letter);
            int index = letter - 'A';
            total[index]++;
        }
    }
    for (int i = 0; i < 26; i++)
    {
        cout << " " << char(i + 'A') << " occurs " <<
            setw(5) << total[i] << " times" << endl;
    }
    double start_time = omp_get_wtime();
    sum = serial_sum(numbers, total[25]);
    double finish_time = omp_get_wtime();
    double compute_time = finish_time - start_time;
    cout << "Serial time =   " << compute_time << " seconds" << endl;
    start_time = omp_get_wtime();
    sum = parallel_sum(numbers, total[25], thread_count);
    finish_time = omp_get_wtime();
    compute_time = finish_time - start_time;
    cout << "Parallel 1 time = " << compute_time << " seconds" << endl;
    start_time = omp_get_wtime();
    sum = parallel_sumv2(numbers, total[25], thread_count);
    finish_time = omp_get_wtime();
    compute_time = finish_time - start_time;
    cout << "Parallel 2 time = " << compute_time << " seconds" << endl;
    start_time = omp_get_wtime();
    sum = parallel_sumv3(numbers, total[25], thread_count);
    finish_time = omp_get_wtime();
    compute_time = finish_time - start_time;
    cout << "Parallel 3 time = " << compute_time << " seconds" << endl;
    start_time = omp_get_wtime();
    sum = parallel_sumv4(numbers, total[25], thread_count);
    finish_time = omp_get_wtime();
    compute_time = finish_time - start_time;
    cout << "Parallel 4 time = " << compute_time << " seconds" << endl;

    cout << "done";
    getchar();

    return 0; 
}
