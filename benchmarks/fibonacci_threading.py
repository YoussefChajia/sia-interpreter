# Benchmark: Parallel Recursive Fibonacci

import threading

# --- Configuration ---
fib_number = 30
num_tasks = 4

fib_number_task1 = fib_number
fib_number_task2 = fib_number - 5
fib_number_task3 = fib_number
fib_number_task4 = fib_number - 10

# --- Fibonacci function ---
def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

# --- Parallel execution ---
print(f"Starting {num_tasks} parallel Fibonacci tasks...")

# Define task functions for each thread
def task1():
    global task1_result
    print(f"Parallel Fib Task 1 starting (n={fib_number_task1})")
    task1_result = fibonacci(fib_number_task1)
    print(f"Parallel Fib Task 1 finished: {task1_result}")

def task2():
    global task2_result
    print(f"Parallel Fib Task 2 starting (n={fib_number_task2})")
    task2_result = fibonacci(fib_number_task2)
    print(f"Parallel Fib Task 2 finished: {task2_result}")

def task3():
    global task3_result
    print(f"Parallel Fib Task 3 starting (n={fib_number_task3})")
    task3_result = fibonacci(fib_number_task3)
    print(f"Parallel Fib Task 3 finished: {task3_result}")

def task4():
    global task4_result
    print(f"Parallel Fib Task 4 starting (n={fib_number_task4})")
    task4_result = fibonacci(fib_number_task4)
    print(f"Parallel Fib Task 4 finished: {task4_result}")

# Create threads
thread1 = threading.Thread(target=task1)
thread2 = threading.Thread(target=task2)
thread3 = threading.Thread(target=task3)
thread4 = threading.Thread(target=task4)

# Start threads
thread1.start()
thread2.start()
thread3.start()
thread4.start()

# Wait for all threads to complete
thread1.join()
thread2.join()
thread3.join()
thread4.join()

print("Parallel Fibonacci tasks completed.")

