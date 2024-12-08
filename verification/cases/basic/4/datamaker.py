
import random

for i in range(1, 11):
    with open(f"{i}.in", "w") as f:
        n = random.randint(1, 1000)
        f.write(f"{n}\n")
        for _ in range(n):
            f.write(f"{random.randint(0, 1000)}\n")