import subprocess
import time
sequentialTime = 0
def sequential():
    global sequentialTime
    with open("sequential.txt", "a") as file:
        file.write("Sequential")
        c = 0
        totalTime = 0
        while c < 10:
            start = time.time()
            subprocess.run("../provaSequenziale/cmake-build-release/provaSequenziale 1000000 500", shell=True)
            end = time.time()
            totalTime += end-start
            file.write("Value : {}s\n".format(end-start))
            c += 1
            print("Iteration  ", c, " time ", end - start)
        averageTime = totalTime / 10
        sequentialTime = totalTime / 10
        file.write("Average execution time: {}s\n".format(averageTime))

def parallel():
    global sequentialTime
    numThread = 1
    with open("parallel.txt", "a") as file:
        totalTime = 0
        while numThread < 257:
            file.write("Threads number : {}\n".format(numThread))
            c = 0
            while c < 10:
                start = time.time()
                subprocess.run(("../provaParallela3/cmake-build-release/provaParallela3 1000000 500 %d" % numThread), shell=True)
                end = time.time()
                totalTime += end-start
                file.write("Value : {}s\n".format(end-start))
                c += 1
                print("Iteration ", c, " time ", end - start)
            averageTime = totalTime / 10
            totalTime = 0
            speedUp = sequentialTime / averageTime
            print("-- thread -- ", numThread)
            print("Average execution time: {}s ".format(averageTime))
            print("-- speedUp -- ", speedUp)
            file.write("Average execution time: {}s\n".format(averageTime))
            file.write("SPEEDUP: {}\n".format(speedUp))
            numThread *= 2

print("Starting sequential...\n")
sequential()
print("Starting parallel...\n")
parallel()