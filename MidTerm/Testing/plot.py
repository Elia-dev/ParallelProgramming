import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import csv
def leggi_file_csv(nome_file):
    punti = []
    centroidi = []

    with open(nome_file, 'r') as file:
        reader = csv.reader(file)
        for riga in reader:
            elemento = list(map(float, riga))
            if len(elemento) == 3:  # Se la terza coordinata è intera, è un punto
                punti.append(elemento)
            else:
                centroidi.append(elemento)
    return punti, centroidi

def disegna_punti_e_centroidi(punti, centroidi):

    punti_x = [punto[0] for punto in punti]
    punti_y = [punto[1] for punto in punti]

    centroidi_x = [centroide[0] for centroide in centroidi]
    centroidi_y = [centroide[1] for centroide in centroidi]

    # Colore in base al cluster di appartenenza per i punti
    #colori = ['c', 'y', 'b', 'orange', 'm', 'g', 'k', 'purple']  # Possibili colori per i cluster
    all_colors = list(mcolors.CSS4_COLORS.keys())
    colori=all_colors[:50]

    plt.scatter(punti_x, punti_y, c=[colori[int(punto[2])] for punto in punti], label='Punti')
    plt.scatter(centroidi_x, centroidi_y, c='red', marker='x', label='Centroidi')

    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Punti e Centroidi')
    plt.grid(True)
    plt.show()

#nome_file_csv = ("../provaParallela2/cmake-build-release/output50.csv")
nome_file_csv = ("../provaParallela3/cmake-build-release/output50.csv")
punti, centroidi = leggi_file_csv(nome_file_csv)
disegna_punti_e_centroidi(punti, centroidi)