#!/bin/bash

# Benchmark script for Python, Java and Sia performance comparison
# Author: Generated for thesis on "Design and Implementation of an Interpreted Programming Language with Native Support for True Multi-threading"
# Date: April 26, 2025

# Configuration
ITERATIONS=5  # Number of times to run each test for averaging
OUTPUT_FILE="benchmark_results.csv"
TIME_FORMAT="%e"  # Real elapsed time in seconds

# Check if Sia interpreter exists
if ! command -v sia &> /dev/null; then
    echo "Error: Sia interpreter not found. Please ensure it's installed and in your PATH."
    exit 1
fi

# Check if Java is installed
if ! command -v java &> /dev/null; then
    echo "Error: Java not found. Please ensure it's installed and in your PATH."
    exit 1
fi

# Create CSV header
echo "Language,Test,Mode,Iteration,Duration" > $OUTPUT_FILE

# Function to run a test and record timing
run_test() {
    local language=$1
    local test_name=$2
    local mode=$3
    local file=$4
    
    for i in $(seq 1 $ITERATIONS); do
        echo "Running $language $test_name $mode (iteration $i of $ITERATIONS)..."
        
        # Create a temporary file for the time output
        temp_file=$(mktemp)
        
        # Time the execution using the time command
        if [ "$language" == "Python" ]; then
            /usr/bin/time -f "$TIME_FORMAT" -o "$temp_file" python3 "$file" 2>/dev/null
        elif [ "$language" == "Java" ]; then
            # Compile Java program first if class file doesn't exist or is older than the source
            class_file="${file%.java}.class"
            if [ ! -f "$class_file" ] || [ "$file" -nt "$class_file" ]; then
                echo "Compiling $file..."
                javac "$file"
            fi
            class_name=$(basename "$file" .java)
            /usr/bin/time -f "$TIME_FORMAT" -o "$temp_file" java "$class_name" 2>/dev/null
        else  # Sia
            /usr/bin/time -f "$TIME_FORMAT" -o "$temp_file" sia "$file" 2>/dev/null
        fi
        
        # Read the duration from the temporary file
        duration=$(cat "$temp_file")
        rm "$temp_file"
        
        # Record the results
        echo "$language,$test_name,$mode,$i,$duration" >> $OUTPUT_FILE
        
        # Add a short delay between tests
        sleep 1
    done
}

# ======= Run Fibonacci Tests =======
echo "===== Running Fibonacci Benchmarks ====="

# Python Fibonacci - Sequential
run_test "Python" "Fibonacci" "Sequential" "fibonacci_sequential.py"

# Python Fibonacci - Threading
run_test "Python" "Fibonacci" "Threading" "fibonacci_threading.py"

# Python Fibonacci - Multiprocessing
run_test "Python" "Fibonacci" "Multiprocessing" "fibonacci_multiprocessing.py"

# Java Fibonacci - Sequential
run_test "Java" "Fibonacci" "Sequential" "fibonacci_sequential.java"

# Java Fibonacci - Threads
run_test "Java" "Fibonacci" "Threads" "fibonacci_parallel.java"

# Sia Fibonacci - Sequential
run_test "Sia" "Fibonacci" "Sequential" "fibonacci_sequential.sia"

# Sia Fibonacci - Parallel
run_test "Sia" "Fibonacci" "Parallel" "fibonacci_parallel.sia"

# ======= Run Iterative Sum Tests =======
echo "===== Running Iterative Sum Benchmarks ====="

# Python Iterative Sum - Sequential
run_test "Python" "IterativeSum" "Sequential" "iterative_sum_sequential.py"

# Python Iterative Sum - Threading
run_test "Python" "IterativeSum" "Threading" "iterative_sum_threading.py"

# Python Iterative Sum - Multiprocessing
run_test "Python" "IterativeSum" "Multiprocessing" "iterative_sum_multiprocessing.py"

# Java Iterative Sum - Sequential
run_test "Java" "IterativeSum" "Sequential" "iterative_sum_sequential.java"

# Java Iterative Sum - Threads
run_test "Java" "IterativeSum" "Threads" "iterative_sum_parallel.java"

# Sia Iterative Sum - Sequential
run_test "Sia" "IterativeSum" "Sequential" "iterative_sum_sequential.sia"

# Sia Iterative Sum - Parallel
run_test "Sia" "IterativeSum" "Parallel" "iterative_sum_parallel.sia"

# ======= Generate Summary Statistics =======
echo "Calculating average execution times..."

echo "Test,Mode,Language,Average Duration" > benchmark_summary.csv

# Process Python Fibonacci Sequential
python_fib_seq=$(awk -F, '$1=="Python" && $2=="Fibonacci" && $3=="Sequential" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "Fibonacci,Sequential,Python,$python_fib_seq" >> benchmark_summary.csv

# Process Python Fibonacci Threading
python_fib_thread=$(awk -F, '$1=="Python" && $2=="Fibonacci" && $3=="Threading" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "Fibonacci,Threading,Python,$python_fib_thread" >> benchmark_summary.csv

# Process Python Fibonacci Multiprocessing
python_fib_multiproc=$(awk -F, '$1=="Python" && $2=="Fibonacci" && $3=="Multiprocessing" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "Fibonacci,Multiprocessing,Python,$python_fib_multiproc" >> benchmark_summary.csv

# Process Java Fibonacci Sequential
java_fib_seq=$(awk -F, '$1=="Java" && $2=="Fibonacci" && $3=="Sequential" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "Fibonacci,Sequential,Java,$java_fib_seq" >> benchmark_summary.csv

# Process Java Fibonacci Threads
java_fib_threads=$(awk -F, '$1=="Java" && $2=="Fibonacci" && $3=="Threads" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "Fibonacci,Threads,Java,$java_fib_threads" >> benchmark_summary.csv

# Process Sia Fibonacci Sequential
sia_fib_seq=$(awk -F, '$1=="Sia" && $2=="Fibonacci" && $3=="Sequential" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "Fibonacci,Sequential,Sia,$sia_fib_seq" >> benchmark_summary.csv

# Process Sia Fibonacci Parallel
sia_fib_par=$(awk -F, '$1=="Sia" && $2=="Fibonacci" && $3=="Parallel" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "Fibonacci,Parallel,Sia,$sia_fib_par" >> benchmark_summary.csv

# Process Python IterativeSum Sequential
python_sum_seq=$(awk -F, '$1=="Python" && $2=="IterativeSum" && $3=="Sequential" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "IterativeSum,Sequential,Python,$python_sum_seq" >> benchmark_summary.csv

# Process Python IterativeSum Threading
python_sum_thread=$(awk -F, '$1=="Python" && $2=="IterativeSum" && $3=="Threading" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "IterativeSum,Threading,Python,$python_sum_thread" >> benchmark_summary.csv

# Process Python IterativeSum Multiprocessing
python_sum_multiproc=$(awk -F, '$1=="Python" && $2=="IterativeSum" && $3=="Multiprocessing" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "IterativeSum,Multiprocessing,Python,$python_sum_multiproc" >> benchmark_summary.csv

# Process Java IterativeSum Sequential
java_sum_seq=$(awk -F, '$1=="Java" && $2=="IterativeSum" && $3=="Sequential" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "IterativeSum,Sequential,Java,$java_sum_seq" >> benchmark_summary.csv

# Process Java IterativeSum Threads
java_sum_threads=$(awk -F, '$1=="Java" && $2=="IterativeSum" && $3=="Threads" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "IterativeSum,Threads,Java,$java_sum_threads" >> benchmark_summary.csv

# Process Sia IterativeSum Sequential
sia_sum_seq=$(awk -F, '$1=="Sia" && $2=="IterativeSum" && $3=="Sequential" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "IterativeSum,Sequential,Sia,$sia_sum_seq" >> benchmark_summary.csv

# Process Sia IterativeSum Parallel
sia_sum_par=$(awk -F, '$1=="Sia" && $2=="IterativeSum" && $3=="Parallel" {sum+=$5; count++} END {print sum/count}' $OUTPUT_FILE)
echo "IterativeSum,Parallel,Sia,$sia_sum_par" >> benchmark_summary.csv

# Calculate speedups
echo -e "\nCalculating parallel speedups..."
echo "Test,Language,Sequential,Mode,Parallel,Speedup" > speedup_results.csv

# Fibonacci Python Threading Speedup
python_fib_thread_speedup=$(echo "$python_fib_seq / $python_fib_thread" | bc -l)
echo "Fibonacci,Python,$python_fib_seq,Threading,$python_fib_thread,$python_fib_thread_speedup" >> speedup_results.csv

# Fibonacci Python Multiprocessing Speedup
python_fib_multiproc_speedup=$(echo "$python_fib_seq / $python_fib_multiproc" | bc -l)
echo "Fibonacci,Python,$python_fib_seq,Multiprocessing,$python_fib_multiproc,$python_fib_multiproc_speedup" >> speedup_results.csv

# Fibonacci Java Threads Speedup
java_fib_threads_speedup=$(echo "$java_fib_seq / $java_fib_threads" | bc -l)
echo "Fibonacci,Java,$java_fib_seq,Threads,$java_fib_threads,$java_fib_threads_speedup" >> speedup_results.csv

# Fibonacci Sia Speedup
sia_fib_speedup=$(echo "$sia_fib_seq / $sia_fib_par" | bc -l)
echo "Fibonacci,Sia,$sia_fib_seq,Parallel,$sia_fib_par,$sia_fib_speedup" >> speedup_results.csv

# IterativeSum Python Threading Speedup
python_sum_thread_speedup=$(echo "$python_sum_seq / $python_sum_thread" | bc -l)
echo "IterativeSum,Python,$python_sum_seq,Threading,$python_sum_thread,$python_sum_thread_speedup" >> speedup_results.csv

# IterativeSum Python Multiprocessing Speedup
python_sum_multiproc_speedup=$(echo "$python_sum_seq / $python_sum_multiproc" | bc -l)
echo "IterativeSum,Python,$python_sum_seq,Multiprocessing,$python_sum_multiproc,$python_sum_multiproc_speedup" >> speedup_results.csv

# IterativeSum Java Threads Speedup
java_sum_threads_speedup=$(echo "$java_sum_seq / $java_sum_threads" | bc -l)
echo "IterativeSum,Java,$java_sum_seq,Threads,$java_sum_threads,$java_sum_threads_speedup" >> speedup_results.csv

# IterativeSum Sia Speedup
sia_sum_speedup=$(echo "$sia_sum_seq / $sia_sum_par" | bc -l)
echo "IterativeSum,Sia,$sia_sum_seq,Parallel,$sia_sum_par,$sia_sum_speedup" >> speedup_results.csv

# Clean up compiled binaries
echo "Cleaning up compiled binaries..."
rm -f *.class

echo "All benchmarks completed!"
echo "Raw results stored in: $OUTPUT_FILE"
echo "Summary statistics stored in: benchmark_summary.csv"
echo "Speedup results stored in: speedup_results.csv"
