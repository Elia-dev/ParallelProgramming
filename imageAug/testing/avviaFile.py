import subprocess
import time
sequentialTime = 0
numEseq = 10
numAug = 10
def sequential():
    global sequentialTime
    global numEseq
    global numAug
    with (open("sequential.txt", "a") as file):
        file.write("Sequential")
        c = 0
        totalTime = 0
        while c < numEseq:
            start = time.time()
            subprocess.run(['python3', 'Sequential_aug.py', str(numAug)], stdout=subprocess.PIPE)
            end = time.time()
            totalTime += end-start
            file.write("Value : {}s\n".format(end-start))
            c += 1
            print("Iteration  ", c, " time ", end - start)
        averageTime = totalTime / numEseq
        sequentialTime = totalTime / numEseq
        file.write("Average execution time: {}s\n".format(averageTime))

def parallel():
    global sequentialTime
    global numAug
    global numEseq
    numProcess = 1
    with open("parallel.txt", "a") as file:
        totalTime = 0
        while numProcess < 33:
            print("-- thread -- ", numProcess)
            file.write("Threads number : {}\n".format(numProcess))
            c = 0
            while c < numEseq:
                start = time.time()
                subprocess.run(['python3', 'Parallel_augmentation.py', str(numAug), str(numProcess)], stdout=subprocess.PIPE)
                end = time.time()
                totalTime += end-start
                file.write("Value : {}s\n".format(end-start))
                c += 1
                print("Iteration ", c, " time ", end - start)
            averageTime = totalTime / numEseq
            totalTime = 0
            speedUp = sequentialTime / averageTime

            print("Average execution time: {}s ".format(averageTime))
            print("-- speedUp -- ", speedUp)
            file.write("Average execution time: {}s\n".format(averageTime))
            file.write("SPEEDUP: {}\n".format(speedUp))
            numProcess *= 2

print("Starting sequential...\n")
sequential()
print("Starting parallel...\n")
parallel()