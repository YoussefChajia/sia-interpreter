# Benchmark: Parallel Iterative Summation
from multiprocessing import Pool, Process, Manager
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

# Function for named processes
def task_worker(task_id, results_dict):
    print(f"Parallel Sum Task {task_id} starting")
    result = iterative_sum(sum_limit)
    results_dict[f"task{task_id}_result"] = result
    print(f"Parallel Sum Task {task_id} finished: {result}")

# --- Parallel execution ---
if __name__ == "__main__":
    print(f"Starting {num_tasks} parallel summation tasks up to {sum_limit}")
    
    # Using Manager to share results between processes
    with Manager() as manager:
        results = manager.dict()
        
        # Create processes
        processes = []
        for i in range(1, num_tasks + 1):
            p = Process(target=task_worker, args=(i, results))
            processes.append(p)
        
        # Start all processes
        for p in processes:
            p.start()
        
        # Wait for all processes to complete
        for p in processes:
            p.join()
        
        # Access results
        task1_result = results.get("task1_result")
        task2_result = results.get("task2_result")
        task3_result = results.get("task3_result")
        task4_result = results.get("task4_result")
        
    print("Parallel summation tasks completed.")
