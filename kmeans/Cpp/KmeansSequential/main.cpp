/** K-means sequential algorithm
 *
 *  @author Elia Matteini, Filippo Zaccari
 *
 */

#include <ctime>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cmath>

//#define DEBUG_SEED
//#define DEBUG_CSV

using namespace std;

class Point {
private:
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

    double getX() {
        return this->x;
    }

    void setX(double x) {
        this->x = x;
    }

    double getY() {
        return y;
    }

    void setY(double y) {
        this->y = y;
    }

    double getMinDistance() {
        return this->minDistance;
    }
    void setMinDistance(double minDistance) {
        this->minDistance = minDistance;
    }
    void setCluster(int cluster) {
        this->cluster = cluster;
    }

    int getCluster() {
        return this->cluster;
    }

    // Function that evaluate the distance from a generic point p using Euclidean distance
    double evaluateDistance(Point p) {
        return (pow(x - p.getX(), 2) + pow(y - p.getY(), 2));
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
        outputFile << point.getX() << "," << point.getY() << "," << point.getCluster() << endl;
    }

    for (auto &centroid : *centroids) {
        outputFile << centroid.getX() << "," << centroid.getY() << endl;
    }
    outputFile.close();
    cout << "Matrix has been written to " << filename << endl;
}

void generate_random_points(vector<Point> *points, int n) {
#ifdef DEBUG_SEED
    srand(1);
#endif
#ifndef DEBUG_SEED
    srand(time(NULL)); // Generating random seed for random numbers
#endif

    points->reserve(n);
    for(int i = 0; i < n; i++) {
        points->emplace_back(rand() % 6000,rand() % 6000);
    }
}

int main(int argc, char const *argv[]) {
#ifdef DEBUG_CSV
    system("rm output*.csv");
    int globalCount = 0;
#endif
    vector<double> executions;
    vector<Point> points;
    vector<Point> centroids;
    generate_random_points(&points, atoi(argv[1]));
    int centroidsNumber = atoi(argv[2]);
    double sumXYcount[3*centroidsNumber];
    bool control = true;
    /*
    * 1) Initialize k- clusters
    * 2) Compute the centroid of each cluster and assign each point to the nearest centroid
    * 3) Redefine the cluster
    * 4) Repeat steps 2 and 3 until the system arrives at an equilibrium state
    */

    /// 1) Initialize k- clusters
    for (int i = 0; i < 3*centroidsNumber; i++) {
        sumXYcount[i] = 0;
    }

    centroids.reserve(centroidsNumber);
    for (int i = 0; i < centroidsNumber; i++)
        centroids.emplace_back(rand() % 6000, rand() % 6000);

#ifdef DEBUG_CSV
    writeMatrixToCSV(&points, &centroids, "output0.csv");
#endif

    while(control) {
        control = false;
        /// 2) Compute the centroid of each cluster and assign each point to the nearest
        for (auto &point: points) {
            point.setMinDistance(__DBL_MAX__); // Reset of minDistance for each iteration
            for (int i = 0; i < centroidsNumber; i++) { // K number of centroids
                int d = point.evaluateDistance(centroids.at(i));
                if (d < point.getMinDistance()) {
                    point.setMinDistance(d);
                    point.setCluster(i);
                }
            }
            sumXYcount[3 * point.getCluster()] += point.getX();
            sumXYcount[3 * point.getCluster() + 1] += point.getY();
            sumXYcount[3 * point.getCluster() + 2]++;
        }
        /// 3) Redefine the cluster
        for (int i = 0; i < centroidsNumber; i++) {
            double averageX = sumXYcount[3 * i] / (int) sumXYcount[3 * i + 2];
            double averageY = sumXYcount[3 * i + 1] / (int) sumXYcount[3 * i + 2];
            if (centroids.at(i).getX() != averageX) {
                centroids.at(i).setX(averageX);
                control = true;
            }
            if (centroids.at(i).getY() != averageY) {
                centroids.at(i).setY(averageY);
                control = true;
            }
            sumXYcount[3 * i] = 0;
            sumXYcount[3 * i + 1] = 0;
            sumXYcount[3 * i + 2] = 0;
        }
#ifdef DEBUG_CSV
        globalCount++;
#endif
    }
#ifdef DEBUG_CSV
    string filename = "output" + to_string(globalCount) + ".csv";
    writeMatrixToCSV(&points, &centroids, filename);
#endif
    return 0;
}