
import random

for i in range(1, 11):
    with open(f"{i}.in", "w") as f:
        value = random.randint(-10**9, 10**9)
        f.write(f"{value}\n")
        value = random.randint(-10**9, 10**9)
        f.write(f"{value}\n")
        