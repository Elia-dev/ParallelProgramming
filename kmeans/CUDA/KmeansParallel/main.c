#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <cuda.h>
#include "openacc.h"

#define DEBUG_SEED
//#define DEBUG_CSV

typedef struct {
    float *x;
    float *y;
    int *clusters;
}Points;

typedef struct {
    float *x;
    float *y;
}Centroids;

void writeMatrixToCSV(float x[], float y[], int numPoints, int cluster[], float xCentroids[], float yCentroids[], int numCentroids, const char *filename) {
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening the file: %s\n", filename);
        return;
    }

    for (int i = 0; i < numPoints; i++) {
        fprintf(fp, "%f,%f,%d\n", x[i], y[i], cluster[i]);
    }

    for (int i = 0; i < numCentroids; i++) {
        fprintf(fp, "%f,%f\n", xCentroids[i], yCentroids[i]);
    }
    printf("Matrix has been written to %s\n", filename);
    fclose(fp);
}

int main(int argc, char const *argv[]) {
#ifdef DEBUG_CSV
    system("rm Poutput*.csv");
    int globalCount = 0;
#endif

    int centroidsNumber = atoi(argv[2]);
    int numPoints = atoi(argv[1]);

    Points points;
    points.x = (float *)malloc(numPoints * sizeof(float));
    points.y = (float *)malloc(numPoints * sizeof(float));
    points.clusters = (int *)malloc(numPoints * sizeof(int));

    Centroids centroids;
    centroids.x = (float *)malloc(centroidsNumber * sizeof(float));
    centroids.y = (float *)malloc(centroidsNumber * sizeof(float));

    int changed = 1;
    double sumXYCount[3*centroidsNumber];

#ifdef DEBUG_SEED
    srand(1);
#endif
#ifndef DEBUG_SEED
    srand(time(NULL)); // Generating random seed for random numbers
#endif

    for(int i = 0; i < numPoints; i++) {
        points.x[i] = rand() % 6000;
        points.y[i] = rand() % 6000;
        points.clusters[i] = -1;
    }

    /*
     * 1) Initialize k- clusters
     * 2) Compute the centroid of each cluster and assign each point to the nearest centroid
     * 3) Redefine the cluster
     * 4) Repeat steps 2 and 3 until the system arrives at an equilibrium state
     */

    /// 1) Initialize k- clusters
    for (int i = 0; i < centroidsNumber; i++) {
        centroids.x[i] = rand() % 6000;
        centroids.y[i] = rand() % 6000;
    }

#ifdef DEBUG_CSV
    writeMatrixToCSV(points.x, points.y, numPoints, points.clusters, centroids.x, centroids.y, centroidsNumber, "Poutput0.csv");
#endif
#pragma acc data copyin(points.x[:numPoints], points.y[:numPoints], centroidsNumber, numPoints, points.clusters[:numPoints], centroids.x[:centroidsNumber], centroids.y[:centroidsNumber], changed) create(sumXYCount[:centroidsNumber*3])
    {
        do {
            changed = 0;

            for (int i = 0; i < 3 * centroidsNumber; i++) {
                sumXYCount[i] = 0;
            }
            /// 2) Compute the centroid of each cluster and assign each point to the nearest

#pragma acc parallel loop gang worker vector reduction(|:changed) reduction(+:sumXYCount[:3*centroidsNumber]) present(points.x[:numPoints], points.y[:numPoints], centroids.x[:centroidsNumber], centroids.y[:centroidsNumber], points.clusters[:numPoints], sumXYCount[:3*centroidsNumber], changed)
            for (int i = 0; i < numPoints; i++) {
                double minDistance = DBL_MAX; // Reset of minDistance for each iteration
                int cluster = -1;

                for (int j = 0; j < centroidsNumber; j++) {
                    int d = pow(points.x[i] - centroids.x[j], 2) + pow(points.y[i] - centroids.y[j], 2);
                    if (d < minDistance) {
                        minDistance = d;
                        cluster = j;
                    }
                }
                if (cluster != points.clusters[i]) {
                    points.clusters[i] = cluster;
                    changed = 1;
                }
                sumXYCount[3 * cluster] += points.x[i];
                sumXYCount[3 * cluster + 1] += points.y[i];
                sumXYCount[3 * cluster + 2]++;

#pragma acc update self(changed)
            }

            if (changed) {
                #pragma acc parallel loop gang worker vector firstprivate(sumXYCount)
                for (int j = 0; j < centroidsNumber; j++) {
                    centroids.x[j] = sumXYCount[3 * j] / sumXYCount[3 * j + 2];
                    centroids.y[j] = sumXYCount[3 * j + 1] / sumXYCount[3 * j + 2];
                }
            }
#ifdef DEBUG_CSV
            globalCount++;
#endif
/*
#ifdef DEBUG_CSV
        globalCount++;
        char filename[20];
        sprintf(filename, "output%d.csv", globalCount);
        writeMatrixToCSV(points.x, points.y, numPoints, points.clusters, centroids.x, centroids.y, centroidsNumber, filename);
        printf("Iterazioni: %d", globalCount);
#endif*/
        } while (changed);
#pragma acc data copyout(points.clusters[0:numPoints], centroids.x[0:centroidsNumber], centroids.y[0:centroidsNumber])
    }


#ifdef DEBUG_CSV
    char filename[20];
    sprintf(filename, "Poutput%d.csv", globalCount);
    writeMatrixToCSV(points.x, points.y, numPoints, points.clusters, centroids.x, centroids.y, centroidsNumber, filename);
    printf("Iterazioni: %d\n", globalCount);
#endif

    free(points.x);
    free(points.y);
    free(points.clusters);
    free(centroids.x);
    free(centroids.y);
    
    return 0;
}
