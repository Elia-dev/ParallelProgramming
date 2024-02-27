import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import csv
def leggi_file_csv(file_name):
    points = []
    centroids = []

    with open(file_name, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            element = list(map(float, row))
            if len(element) == 3:  # If there is a third value, then it's a point otherwise is a centroid
                points.append(element)
            else:
                centroids.append(element)
    return points, centroids

def disegna_punti_e_centroidi(points, centroids):

    points_x = [point[0] for point in points]
    points_y = [point[1] for point in points]

    centroids_x = [centroid[0] for centroid in centroids]
    centroids_y = [centroid[1] for centroid in centroids]

    # Color based by the associeted cluster
    #colors = ['c', 'y', 'b', 'orange', 'm', 'g', 'k', 'purple']
    all_colors = list(mcolors.CSS4_COLORS.keys())
    colors=all_colors[:50]

    plt.scatter(points_x, points_y, c=[colors[int(point[2])] for point in points], label='Points')
    plt.scatter(centroids_x, centroids_y, c='red', marker='x', label='Centroids')

    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Points and centroids')
    plt.grid(True)
    plt.show()

csv_file_name = ("../KmeansParallel/cmake-build-release/output50.csv")
points, centroids = leggi_file_csv(csv_file_name)
disegna_punti_e_centroidi(points, centroids)
