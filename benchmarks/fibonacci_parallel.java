// Benchmark: Parallel Recursive Fibonacci in Java

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicLong;

public class fibonacci_parallel {
    // --- Configuration ---
    private static final int FIB_NUMBER = 30;
    private static final int NUM_TASKS = 4;
    
    private static final int FIB_NUMBER_TASK1 = FIB_NUMBER;
    private static final int FIB_NUMBER_TASK2 = FIB_NUMBER - 5;
    private static final int FIB_NUMBER_TASK3 = FIB_NUMBER;
    private static final int FIB_NUMBER_TASK4 = FIB_NUMBER - 10;
    
    // Results for each task
    private static final AtomicLong task1Result = new AtomicLong(0);
    private static final AtomicLong task2Result = new AtomicLong(0);
    private static final AtomicLong task3Result = new AtomicLong(0);
    private static final AtomicLong task4Result = new AtomicLong(0);
    
    // --- Fibonacci function ---
    private static long fibonacci(int n) {
        if (n <= 1) {
            return n;
        }
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
    
    // Fibonacci task Thread classes
    static class FibTask1 extends Thread {
        private final CountDownLatch latch;
        
        public FibTask1(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.printf("Parallel Fib Task 1 starting (n=%d)%n", FIB_NUMBER_TASK1);
                long result = fibonacci(FIB_NUMBER_TASK1);
                task1Result.set(result);
                System.out.printf("Parallel Fib Task 1 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    static class FibTask2 extends Thread {
        private final CountDownLatch latch;
        
        public FibTask2(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.printf("Parallel Fib Task 2 starting (n=%d)%n", FIB_NUMBER_TASK2);
                long result = fibonacci(FIB_NUMBER_TASK2);
                task2Result.set(result);
                System.out.printf("Parallel Fib Task 2 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    static class FibTask3 extends Thread {
        private final CountDownLatch latch;
        
        public FibTask3(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.printf("Parallel Fib Task 3 starting (n=%d)%n", FIB_NUMBER_TASK3);
                long result = fibonacci(FIB_NUMBER_TASK3);
                task3Result.set(result);
                System.out.printf("Parallel Fib Task 3 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    static class FibTask4 extends Thread {
        private final CountDownLatch latch;
        
        public FibTask4(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.printf("Parallel Fib Task 4 starting (n=%d)%n", FIB_NUMBER_TASK4);
                long result = fibonacci(FIB_NUMBER_TASK4);
                task4Result.set(result);
                System.out.printf("Parallel Fib Task 4 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    public static void main(String[] args) throws InterruptedException {
        System.out.printf("Starting %d parallel Fibonacci tasks...%n", NUM_TASKS);
        
        // Use CountDownLatch to wait for all tasks to complete
        CountDownLatch latch = new CountDownLatch(NUM_TASKS);
        
        // Create and start threads
        Thread thread1 = new FibTask1(latch);
        Thread thread2 = new FibTask2(latch);
        Thread thread3 = new FibTask3(latch);
        Thread thread4 = new FibTask4(latch);
        
        thread1.start();
        thread2.start();
        thread3.start();
        thread4.start();
        
        // Wait for all threads to complete
        latch.await();
        
        System.out.println("Parallel Fibonacci tasks completed.");
    }
} 