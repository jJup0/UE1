import random
from copy import deepcopy


def randRange(x):
    l = list(range(x))
    random.shuffle(l)
    return l


def re_init():
    return m_global, n_global, deepcopy(M_global), V_global.copy(), W_global.copy()


def alg1():
    m, n, M, V, W = re_init()
    for i in randRange(m):
        W[i] = 0
        for j in randRange(n):
            # summing into each cell in result vector
            W[i] += M[i][j] * V[j]

    return W


def alg2():
    m, n, M, V, W = re_init()
    for i in randRange(m):
        for j in randRange(n):
            # calculating summands in O(1)time
            M[i][j] *= V[j]

    sum_matrix_into_vector(M, m, n, W)

    return W


def alg3():
    m, n, M, V, W = re_init()
    C = [[0] * n for _ in randRange(m)]

    for j in randRange(n):
        C[0][j] = V[j]

    offset = 1
    while(offset < m):
        # duplicating existing vectors in O(1) time
        for i in randRange(m):
            if (i + offset < m):
                for j in randRange(n):
                    C[i + offset][j] = C[i][j]

        offset *= 2

    for i in randRange(m):
        for j in randRange(n):
            # calculating summands in O(1)time
            M[i][j] = M[i][j] * C[i][j]

    sum_matrix_into_vector(M, m, n, W)

    return W


def sum_matrix_into_vector(M, m, n, W):
    for i in randRange(m):
        # summing in O(log(n)) time
        offset = 1
        betweenGap = 2

        while (offset < n):
            for j in randRange(n):
                if(offset + j * betweenGap < n):
                    M[i][j * betweenGap] += M[i][offset + j * betweenGap]
            offset *= 2
            betweenGap *= 2

    for i in randRange(m):
        # copying into result vector in O(1) time
        W[i] = M[i][0]

    return W


random.seed(1)
# m_global = n = M = V = W = C = None


for i in range(100):
    m_global = random.randint(1, 99)
    n_global = random.randint(1, 99)
    M_global = [[random.randint(-100, 100) for _ in range(n_global)] for _ in range(m_global)]
    V_global = [random.randint(-100, 100) for _ in range(n_global)]
    W_global = [0] * m_global

    res1 = alg1()
    res2 = alg2()
    res3 = alg3()
    # print(res1)
    # print(res2)
    # print(res3)
    print(f"run i={i}")
    if (res1 != res2):
        print(f"run i={i} failed for alg2")
    if (res1 != res3):
        print(f"run i={i} failed for alg3")
