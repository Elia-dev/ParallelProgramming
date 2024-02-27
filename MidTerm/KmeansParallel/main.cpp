/** K-means parallel algorithm
 *
 *  @author Elia Matteini, Filippo Zaccari
 *
 */

#include <omp.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>

//#define DEBUG_SEED
//#define DEBUG_CSV
//#define DEBUG_OPENMP
#define PARALLEL

using namespace std;

class Point {
public:
    double x, y;     // Point's coordinates
    int cluster;     // Cluster assigned to the point
    double minDistance; // Distance from the cluster
public:
    Point(double x, double y) {
        this->x = x;
        this->y = y;
        this->minDistance = __DBL_MAX__; // Set the default distance from the nearest cluster
        // equals to the maximum representable floating point number
        this->cluster = -1; // Initial cluster is null, first cluster is the number zero
    }
};

// Function to write matrix into a CSV file
void writeMatrixToCSV(vector<Point> *points, vector<Point> *centroids, string filename) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening the file: " << filename << endl;
        return;
    }

    for (auto &point: *points) {
        outputFile << point.x << "," << point.y << "," << point.cluster << endl;
    }

    for (auto &centroid : *centroids) {
        outputFile << centroid.x << "," << centroid.y << endl;
    }
    outputFile.close();
    cout << "Matrix has been written to " << filename << endl;
}

int main(int argc, char const *argv[]) {
#ifdef DEBUG_OPENMP
    cout << "_OPENMP defined" << endl;
    cout << "Num processors (Phys+HT): " << omp_get_num_procs() << endl;
#endif
#ifdef PARALLEL
    omp_set_num_threads(atoi(argv[3]));
#endif
#ifdef DEBUG_CSV
    system("rm output*.csv");
    int globalCount = 0;
#endif

    int centroidsNumber = atoi(argv[2]);
    int numPoints = atoi(argv[1]);
    bool changed = true;
    vector<Point> points;
    vector<Point> centroids;
    double sumXYCount[3*centroidsNumber];
#ifdef DEBUG_SEED
    srand(1);
#endif
#ifndef DEBUG_SEED
    srand(time(NULL)); // Generating random seed for random numbers
#endif

    points.reserve(numPoints);
    for(int i = 0; i < numPoints; i++) {
        points.emplace_back(rand() % 6000,rand() % 6000);
    }

    /*
     * 1) Initialize k- clusters
     * 2) Compute the centroid of each cluster and assign each point to the nearest centroid
     * 3) Redefine the cluster
     * 4) Repeat steps 2 and 3 until the system arrives at an equilibrium state
     */

    /// 1) Initialize k- clusters
    centroids.reserve(centroidsNumber);
    for (int i = 0; i < centroidsNumber; i++) {
        centroids.emplace_back(rand() % 6000, rand() % 6000);
    }
#ifdef DEBUG_CSV
    writeMatrixToCSV(&points, &centroids, "output0.csv");
#endif

    do {
        changed = false;
        for(int i = 0; i < 3*centroidsNumber; i++) {
            sumXYCount[i] = 0;
        }
        /// 2) Compute the centroid of each cluster and assign each point to the nearest
#ifdef PARALLEL
#pragma omp parallel for reduction(+:sumXYCount[:3*centroidsNumber])
#endif
        for (int i = 0; i < numPoints; i++) {
            points[i].minDistance = __DBL_MAX__; // Reset of minDistance for each iteration
            int cluster = -1;
            for (int j = 0; j < centroidsNumber; j++) {
                int d = pow(points[i].x - centroids[j].x, 2) + pow(points[i].y - centroids[j].y, 2);
                if (d < points[i].minDistance) {
                    points[i].minDistance = d;
                    cluster = j;
                }
            }
            if(cluster != points[i].cluster) {
                points[i].cluster = cluster;
                changed = true;
            }
            sumXYCount[3 * cluster] += points[i].x;
            sumXYCount[3 * cluster + 1] += points[i].y;
            sumXYCount[3 * cluster + 2]++;
        }

        if(changed) {
            /// 3) Redefine the cluster

#ifdef PARALLEL
#pragma omp parallel for
#endif
            for (int j = 0; j < centroidsNumber; j++) {
                centroids[j].x = sumXYCount[3 * j] / sumXYCount[3 * j + 2];
                centroids[j].y = sumXYCount[3 * j + 1] / sumXYCount[3 * j + 2];
            }
        }

#ifdef DEBUG_CSV
        globalCount++;
#endif
    }while(changed);

#ifdef DEBUG_CSV
    string filename = "output" + to_string(globalCount) + ".csv";
    writeMatrixToCSV(&points, &centroids, filename);
    printf("Iterazioni: %d", globalCount);
    cout << "GlobalCount: " << globalCount << endl;
#endif
    return 0;
}
