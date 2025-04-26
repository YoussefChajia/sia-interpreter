# Benchmark: Parallel Iterative Summation with Threading
import threading
import time

# --- Configuration ---
sum_limit = 1000000
num_tasks = 4

# --- Iterative Sum function ---
def iterative_sum(limit):
    total = 0
    i = 0
    for _ in range(limit + 1):
        total += i
        i += 1
    return total

# --- Global results for thread access ---
task1_result = 0
task2_result = 0
task3_result = 0
task4_result = 0

# --- Define task functions for each thread ---
def task1():
    global task1_result
    print(f"Parallel Sum Task 1 starting")
    task1_result = iterative_sum(sum_limit)
    print(f"Parallel Sum Task 1 finished: {task1_result}")

def task2():
    global task2_result
    print(f"Parallel Sum Task 2 starting")
    task2_result = iterative_sum(sum_limit)
    print(f"Parallel Sum Task 2 finished: {task2_result}")

def task3():
    global task3_result
    print(f"Parallel Sum Task 3 starting")
    task3_result = iterative_sum(sum_limit)
    print(f"Parallel Sum Task 3 finished: {task3_result}")

def task4():
    global task4_result
    print(f"Parallel Sum Task 4 starting")
    task4_result = iterative_sum(sum_limit)
    print(f"Parallel Sum Task 4 finished: {task4_result}")

# --- Parallel execution ---
print(f"Starting {num_tasks} parallel summation tasks up to {sum_limit}")

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

print("Parallel summation tasks completed.") 