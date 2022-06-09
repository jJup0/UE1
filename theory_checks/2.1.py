import random

random.seed(1)

n = 20
a = [random.randint(-10, 10) for _ in range(n)]
b = [0] * n

print(a)

j = 0
for i in range(n):
    count = 0
    for j in range(i):
        if a[j] <= a[i]:
            count += 1
    j += 1
    for j in range(j, n):
        if a[j] < a[i]:
            count += 1
    b[count] = a[i]

for i in range(n):
    a[i] = b[i]

print(a)
