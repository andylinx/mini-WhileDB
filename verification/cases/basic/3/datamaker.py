
import random

for i in range(1, 11):
    with open(f"{i}.in", "w") as f:
        f.write(f"{random.randint(-10000000, 10000000)}\n")