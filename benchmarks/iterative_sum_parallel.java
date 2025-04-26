// Benchmark: Parallel Iterative Summation in Java

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicLong;

public class iterative_sum_parallel {
    // --- Configuration ---
    private static final int SUM_LIMIT = 1000000;
    private static final int NUM_TASKS = 4;
    
    // Results for each task
    private static final AtomicLong task1Result = new AtomicLong(0);
    private static final AtomicLong task2Result = new AtomicLong(0);
    private static final AtomicLong task3Result = new AtomicLong(0);
    private static final AtomicLong task4Result = new AtomicLong(0);
    
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
    
    // Sum task Thread classes
    static class SumTask1 extends Thread {
        private final CountDownLatch latch;
        
        public SumTask1(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.println("Parallel Sum Task 1 starting");
                long result = iterativeSum(SUM_LIMIT);
                task1Result.set(result);
                System.out.printf("Parallel Sum Task 1 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    static class SumTask2 extends Thread {
        private final CountDownLatch latch;
        
        public SumTask2(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.println("Parallel Sum Task 2 starting");
                long result = iterativeSum(SUM_LIMIT);
                task2Result.set(result);
                System.out.printf("Parallel Sum Task 2 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    static class SumTask3 extends Thread {
        private final CountDownLatch latch;
        
        public SumTask3(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.println("Parallel Sum Task 3 starting");
                long result = iterativeSum(SUM_LIMIT);
                task3Result.set(result);
                System.out.printf("Parallel Sum Task 3 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    static class SumTask4 extends Thread {
        private final CountDownLatch latch;
        
        public SumTask4(CountDownLatch latch) {
            this.latch = latch;
        }
        
        @Override
        public void run() {
            try {
                System.out.println("Parallel Sum Task 4 starting");
                long result = iterativeSum(SUM_LIMIT);
                task4Result.set(result);
                System.out.printf("Parallel Sum Task 4 finished: %d%n", result);
            } finally {
                latch.countDown();
            }
        }
    }
    
    public static void main(String[] args) throws InterruptedException {
        System.out.printf("Starting %d parallel summation tasks up to %d...%n", NUM_TASKS, SUM_LIMIT);
        
        // Use CountDownLatch to wait for all tasks to complete
        CountDownLatch latch = new CountDownLatch(NUM_TASKS);
        
        // Create and start threads
        Thread thread1 = new SumTask1(latch);
        Thread thread2 = new SumTask2(latch);
        Thread thread3 = new SumTask3(latch);
        Thread thread4 = new SumTask4(latch);
        
        thread1.start();
        thread2.start();
        thread3.start();
        thread4.start();
        
        // Wait for all threads to complete
        latch.await();
        
        System.out.println("Parallel summation tasks completed.");
    }
} 