import random
from copy import deepcopy

random.seed(1)
m_global = n = M = V = W = C = None
m_global = 79
n_global = 431
M_global = [[random.randint(-10, 10) for _ in range(n_global)] for _ in range(m_global)]
V_global = [random.randint(-10, 10) for _ in range(n_global)]
# M_global = [[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]]
# V_global = [1, 1, 1, 1]
W_global = [0] * m_global

# print(M_global)
# print(V_global)


def re_init():
    return m_global, n_global, deepcopy(M_global), V_global.copy(), W_global.copy()


def alg1():
    m, n, M, V, W = re_init()
    for i in range(m):
        W[i] = 0
        for j in range(n):
            # summing into each cell in result vector
            W[i] += M[i][j] * V[j]

    return W


def alg2():
    m, n, M, V, W = re_init()
    for i in range(m):
        for j in range(n):
            # calculating summands in O(1)time
            M[i][j] *= V[j]

    M_copy = deepcopy(M)
    for i in range(m):
        # summing in O(log(n)) time
        offset = 1
        betweenGap = 2

        while (offset < n):
            for j in range(n):
                if(offset + j * betweenGap < n):
                    M[i][j * betweenGap] += M[i][offset + j * betweenGap]
            offset *= 2
            betweenGap *= 2

    for i in range(m):
        # copying into result vector in O(1) time
        W[i] = M[i][0]

    return W


def alg3():
    m, n, M, V, W = re_init()
    C = [[0] * n for _ in range(m)]

    for j in range(n):
        C[0][j] = V[j]

    for i in range(m):
        for j in range(n):

            # copying vector in O(log(n)) time
            range_ = 2
            offset = 1
            while(offset < m):
                if (i < offset or i + offset >= m):
                    pass
                else:
                    C[i + offset][j] = C[i][j]

                offset *= 2
                range_ *= 2

            # calculating summands in O(1)time
            M[i][j] = M[i][j] * C[i][j]

            # summing in O(log(n)) time
            offset = 1
            betweenGap = 2
            while(offset < n):
                if(offset + j * betweenGap < n):
                    M[i][j * betweenGap] += M[i][offset + j * betweenGap]
                offset *= 2
                betweenGap *= 2

    # copying into result vector in O(1) time
    for i in range(m):
        W[i] = M[i][0]

    return W


print("-----")
res1 = alg1()
res2 = alg2()
res3 = alg3()
print(res1 == res2)
