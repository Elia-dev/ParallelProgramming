import subprocess
import time
sequentialTime = 0
numIteration = 1
numPoints = 1000000
numCentroids = 500
#numWorkers = 1024
#vectorLength = 1
#1 num points
#2 num centroids
#3 num worker
#4 vector length
def sequential():
    global sequentialTime
    global numIteration
    global numPoints
    global numCentroids
    global numWorkers
    global vectorLength

    with open("sequential.txt", "a") as file:
        file.write("numPoints : " + str(numPoints) + " numCentroids: " + str(numCentroids) + "\n")
        c = 0
        totalTime = 0
        while c < numIteration:
            start = time.time()
            subprocess.run("./sequentialCUDA " + str(numPoints) + " " + str(numCentroids), shell=True)
            end = time.time()
            totalTime += end-start
            file.write("Time : {}s\n".format(end-start))
            c += 1
            print("Iteration  ", c, " time ", end - start)
        averageTime = totalTime / numIteration
        sequentialTime = averageTime
        file.write("Average execution time: {}s\n".format(averageTime))
        print("---------------------------------------------------------------------------------------------------------------")
        file.write("--------------------------------------------------------------------------------------------------------------\n")

def parallel():
    global sequentialTime
    global numIteration
    global numPoints
    global numCentroids
    global numWorkers
    global vectorLength

    numProva = 1
    with open("parallel.txt", "a") as file:
        totalTime = 0
        print("Num points: " + str(numPoints) + " numCentroids: " + str(numCentroids) + "\n")
        while numProva < 2:
            file.write("Parallel\n numPoints : " + str(numPoints) + " numCentroids: " + str(numCentroids))
            c = 0
            while c < numIteration:
                start = time.time()
                subprocess.run("./parallelCUDA " + str(numPoints) + " " + str(numCentroids), shell=True)
                end = time.time()
                totalTime += end-start
                file.write("\nTime : {}s\n".format(end-start))
                c += 1
                print("Iteration ", c, " time ", end - start)
            averageTime = totalTime / numIteration
            totalTime = 0
            speedUp = sequentialTime / averageTime
            print("Average execution time: {}s ".format(averageTime))
            print("-- speedUp -- ", speedUp)
            file.write("Average execution time: {}s\n".format(averageTime))
            file.write("SPEEDUP: {}\n".format(speedUp))
            print("---------------")
            numProva += 1
        print("--------------------------------------------------------------------------------")
        file.write("------------------------------------------------------------------------------------")

print("Starting sequential...\n")
sequential()
print("Starting parallel...\n")
parallel()
