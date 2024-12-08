import random
import string

def generate_string(max_length=10000):
    length = random.randint(1, max_length)
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

if __name__ == "__main__":
    for i in range(1, 11):
        test_string = generate_string()
        with open(f"{i}.in", "w") as f:
            f.write(test_string)