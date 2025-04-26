// Benchmark: Sequential Recursive Fibonacci in Java

public class fibonacci_sequential {
    // --- Configuration ---
    private static final int FIB_NUMBER = 30;
    private static final int NUM_TASKS = 4;
    
    private static final int FIB_NUMBER_TASK1 = FIB_NUMBER;
    private static final int FIB_NUMBER_TASK2 = FIB_NUMBER - 5;
    private static final int FIB_NUMBER_TASK3 = FIB_NUMBER;
    private static final int FIB_NUMBER_TASK4 = FIB_NUMBER - 10;
    
    // --- Fibonacci function ---
    private static long fibonacci(int n) {
        if (n <= 1) {
            return n;
        }
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
    
    public static void main(String[] args) {
        System.out.printf("Starting sequential execution of %d Fibonacci tasks...%n", NUM_TASKS);
        
        // Task 1
        System.out.printf("Sequential Fib Task 1 starting (n=%d)%n", FIB_NUMBER_TASK1);
        long task1Result = fibonacci(FIB_NUMBER_TASK1);
        System.out.printf("Sequential Fib Task 1 finished: %d%n", task1Result);
        
        // Task 2
        System.out.printf("Sequential Fib Task 2 starting (n=%d)%n", FIB_NUMBER_TASK2);
        long task2Result = fibonacci(FIB_NUMBER_TASK2);
        System.out.printf("Sequential Fib Task 2 finished: %d%n", task2Result);
        
        // Task 3
        System.out.printf("Sequential Fib Task 3 starting (n=%d)%n", FIB_NUMBER_TASK3);
        long task3Result = fibonacci(FIB_NUMBER_TASK3);
        System.out.printf("Sequential Fib Task 3 finished: %d%n", task3Result);
        
        // Task 4
        System.out.printf("Sequential Fib Task 4 starting (n=%d)%n", FIB_NUMBER_TASK4);
        long task4Result = fibonacci(FIB_NUMBER_TASK4);
        System.out.printf("Sequential Fib Task 4 finished: %d%n", task4Result);
        
        System.out.println("Sequential Fibonacci tasks completed.");
    }
} 