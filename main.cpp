#include <cstdio>
#include <omp.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "graph.h"
#include "log.h"

using std::istringstream;
using std::ofstream;
using std::string;

const string WRITETO = "stat500.csv";
const string READFROM = "graph2.txt";

int main(int argc, char *argv[]) {
    int num_exp;
    istringstream iss_num_exp(argv[1]);
    iss_num_exp >> num_exp;

    ofstream file(WRITETO);
    auto graph = Graph::Create(READFROM);
    auto& src = graph.RandomSrc();
    // std::cout << src.vid << std::endl;  // 
    // graph.SerialBFS(src);
    // std::cout << std::endl;
    // graph.ParallelBFS(src, 12);
    // std::cout << std::endl;
    // graph.SerialBFS(src);

    double start, end;
    for (int num_t = 1; num_t <= 32; num_t++) {
        file << num_t << ",";
        double time_serial = 0, time_parallel = 0;
        for (int i = 1; i <= num_exp; i++) {
            start = omp_get_wtime();
            graph.SerialBFS(src);
            end = omp_get_wtime();
            time_serial += (end - start) * 1000;

            start = omp_get_wtime();
            graph.ParallelBFS(src, num_t);
            end = omp_get_wtime();
            time_parallel += (end - start) * 1000;
        }
        time_serial /= num_exp;
        time_parallel /= num_exp;
        // std::cout << "\tavg_time_serial = " << time_serial / num_exp;
        // std::cout << "\n\tavg_time_parallel = " << time_parallel / num_exp << "\n";
        file << time_serial << ",";
        file << time_parallel << ",";
        file << time_serial / time_parallel << "\n";
    }

    file.close();
    return 0;
}
