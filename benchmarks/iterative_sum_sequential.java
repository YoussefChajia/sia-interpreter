// Benchmark: Sequential Iterative Summation in Java

public class iterative_sum_sequential {
    // --- Configuration ---
    private static final int SUM_LIMIT = 1000000;
    private static final int NUM_TASKS = 4;
    
    // --- Iterative Sum function ---
    private static long iterativeSum(int limit) {
        long total = 0;
        int i = 0;
        for (int j = 0; j <= limit; j++) {
            total += i;
            i++;
        }
        return total;
    }
    
    public static void main(String[] args) {
        System.out.printf("Starting sequential execution of %d summation tasks up to %d...%n", NUM_TASKS, SUM_LIMIT);
        
        // Task 1
        System.out.println("Sequential Sum Task 1 starting");
        long task1Result = iterativeSum(SUM_LIMIT);
        System.out.printf("Sequential Sum Task 1 finished: %d%n", task1Result);
        
        // Task 2
        System.out.println("Sequential Sum Task 2 starting");
        long task2Result = iterativeSum(SUM_LIMIT);
        System.out.printf("Sequential Sum Task 2 finished: %d%n", task2Result);
        
        // Task 3
        System.out.println("Sequential Sum Task 3 starting");
        long task3Result = iterativeSum(SUM_LIMIT);
        System.out.printf("Sequential Sum Task 3 finished: %d%n", task3Result);
        
        // Task 4
        System.out.println("Sequential Sum Task 4 starting");
        long task4Result = iterativeSum(SUM_LIMIT);
        System.out.printf("Sequential Sum Task 4 finished: %d%n", task4Result);
        
        System.out.println("Sequential summation tasks completed.");
    }
} 