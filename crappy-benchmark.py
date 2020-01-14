import time

# from stackoverflow
def current_milli_time(): return int(round(time.time() * 1000))

def factorial(x):
    if x == 1:
        return 1
    return x * factorial(x - 1)

start = current_milli_time()

for t in range(1, 1000):
    factorial(t)

total = start - current_milli_time()

print(f'time taken: {total}ms')
