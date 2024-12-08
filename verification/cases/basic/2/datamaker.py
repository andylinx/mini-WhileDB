import os
import random


# Function to generate random integers
def generate_random_integers(n, lower_bound=1, upper_bound=100):
    return [random.randint(lower_bound, upper_bound) for _ in range(n)]

# Define the content for each file
file_contents = {
    "1.in": (5, generate_random_integers(5)),
    "2.in": (3, generate_random_integers(3)),
    "3.in": (4, generate_random_integers(4)),
    "4.in": (6, generate_random_integers(6)),
    "5.in": (2, generate_random_integers(2)),
    "6.in": (1, generate_random_integers(1)),
    "7.in": (8, generate_random_integers(8)),
    "8.in": (10, generate_random_integers(10)),
    "9.in": (100, generate_random_integers(100)),
    "10.in": (100000, generate_random_integers(100))
}

# Create each file with the specified content
for filename, (n, integers) in file_contents.items():
    with open(filename, "w") as file:
        file.write(f"{n}\n")
        file.write(" ".join(map(str, integers)) + "\n")