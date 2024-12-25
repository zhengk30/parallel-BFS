import matplotlib.pyplot as plt
import csv


if __name__ == '__main__':
    file = 'C:\\Users\\86176\\OneDrive\\Desktop\\data.csv'
    num_threads, speedups = [], []

    with open(file) as f:
        reader = csv.reader(f)
        for row in reader:
            num_threads.append(int(row[0]))
            # speedups.append(float(row[1]) / float(row[2]))
            speedups.append(float(row[len(row) - 1]))
    
    plt.plot(num_threads, speedups)
    plt.xlabel('NumThds')
    plt.ylabel('Speedup (SerialT / ParallelT)')
    plt.title('Speedup with 500 experiments')
    plt.show()
