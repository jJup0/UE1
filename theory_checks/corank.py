import math
from typing import List


# returns (corank_a, corank_b)
def corank(i: int, A: List[float], n: int, B: List[float], m: int):
    j: int = min(i, n)
    k: int = i - j  # Because j + k = i
    j_low: int = max(0, i - m)
    k_low: int = 0  # uninit in pseudo code
    delta: int

    while (True):
        if (j > 0 and k < m and A[j - 1] > B[k]):
            delta = math.ceil((j - j_low) / 2)
            k_low = k
            j -= delta
            k += delta
        elif (k > 0 and j < n and B[k - 1] >= A[j]):
            delta = math.ceil((k - k_low) / 2)
            j_low = j
            j += delta
            k -= delta
        else:
            return (j, k)
    


A = list(range(1, 200, 2))
B = list(range(0, 10, 2))
A = [a * 1.0 for a in A]
B = [b * 1.0 for b in B]
n = len(A)
m = len(B)
t = 16
for i in range(t):
    corank(i * (n + m) // t, A, n, B, m)
