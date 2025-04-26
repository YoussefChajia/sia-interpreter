# Benchmark: Sequential Iterative Summation

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

# --- Sequential execution ---
print(f"Starting {num_tasks} sequential summation tasks up to {sum_limit}")

# Task 1
print("Sequential Sum Task 1 starting")
task1_result = iterative_sum(sum_limit)
print(f"Sequential Sum Task 1 finished: {task1_result}")

# Task 2
print("Sequential Sum Task 2 starting")
task2_result = iterative_sum(sum_limit)
print(f"Sequential Sum Task 2 finished: {task2_result}")

# Task 3
print("Sequential Sum Task 3 starting")
task3_result = iterative_sum(sum_limit)
print(f"Sequential Sum Task 3 finished: {task3_result}")

# Task 4
print("Sequential Sum Task 4 starting")
task4_result = iterative_sum(sum_limit)
print(f"Sequential Sum Task 4 finished: {task4_result}")

print("Sequential summation tasks completed.")
