# Benchmark: Parallel Recursive Fibonacci with Multiprocessing

from multiprocessing import Process, Manager
import time

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

# --- Task functions for multiprocessing ---
def task1(results_dict):
    print(f"Parallel Fib Task 1 starting (n={fib_number_task1})")
    results_dict["task1_result"] = fibonacci(fib_number_task1)
    print(f"Parallel Fib Task 1 finished: {results_dict['task1_result']}")

def task2(results_dict):
    print(f"Parallel Fib Task 2 starting (n={fib_number_task2})")
    results_dict["task2_result"] = fibonacci(fib_number_task2)
    print(f"Parallel Fib Task 2 finished: {results_dict['task2_result']}")

def task3(results_dict):
    print(f"Parallel Fib Task 3 starting (n={fib_number_task3})")
    results_dict["task3_result"] = fibonacci(fib_number_task3)
    print(f"Parallel Fib Task 3 finished: {results_dict['task3_result']}")

def task4(results_dict):
    print(f"Parallel Fib Task 4 starting (n={fib_number_task4})")
    results_dict["task4_result"] = fibonacci(fib_number_task4)
    print(f"Parallel Fib Task 4 finished: {results_dict['task4_result']}")

# --- Parallel execution ---
if __name__ == "__main__":
    print(f"Starting {num_tasks} parallel Fibonacci tasks...")
    
    # Using Manager to share results between processes
    with Manager() as manager:
        results = manager.dict()
        
        # Create processes
        process1 = Process(target=task1, args=(results,))
        process2 = Process(target=task2, args=(results,))
        process3 = Process(target=task3, args=(results,))
        process4 = Process(target=task4, args=(results,))
        
        # Start all processes
        process1.start()
        process2.start()
        process3.start()
        process4.start()
        
        # Wait for all processes to complete
        process1.join()
        process2.join()
        process3.join()
        process4.join()
        
        # Access results
        task1_result = results.get("task1_result")
        task2_result = results.get("task2_result")
        task3_result = results.get("task3_result")
        task4_result = results.get("task4_result")
    
    print("Parallel Fibonacci tasks completed.") 
