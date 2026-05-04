import numpy as np


def generate_matrix(N: int):
    return np.random.uniform(low=0.0, high=10.0, size=(2*N, N))


def write_matrices_to_file(N: int):
    np.savetxt('matrix{}.dat'.format(N), generate_matrix(N), fmt='%.2f')


write_matrices_to_file(1000)
write_matrices_to_file(2000)
