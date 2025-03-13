#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <future>
#include <vector>

using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { 
                            return stop || !tasks.empty(); 
                        });
                        
                        if (stop && tasks.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    
                    try {
                        task();
                    } catch (const exception& e) {
                        lock_guard<mutex> lock(exception_mutex);
                        if (!exception_ptr) {
                            exception_ptr = make_shared<pair<string, pair<unsigned int, unsigned int>>>(
                                e.what(), make_pair(0, 0)
                            );
                        }
                    }
                }
            });
        }
    }
    
    template<class F>
    auto enqueue(F&& f) -> future<decltype(f())> {
        using return_type = decltype(f());
        
        auto task = make_shared<packaged_task<return_type()>>(forward<F>(f));
        future<return_type> res = task->get_future();
        
        {
            unique_lock<mutex> lock(queue_mutex);
            if (stop) {
                throw runtime_error("enqueue on stopped ThreadPool");
            }
            
            tasks.emplace([task]() { (*task)(); });
        }
        
        condition.notify_one();
        return res;
    }
    
    shared_ptr<pair<string, pair<unsigned int, unsigned int>>> get_exception() const {
        lock_guard<mutex> lock(exception_mutex);
        return exception_ptr;
    }
    
    void wait_all() {
        unique_lock<mutex> lock(queue_mutex);
        condition.wait(lock, [this] { return tasks.empty(); });
    }
    
    ~ThreadPool() {
        {
            unique_lock<mutex> lock(queue_mutex);
            stop = true;
        }
        
        condition.notify_all();
        
        for (thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    
    mutex queue_mutex;
    condition_variable condition;
    atomic<bool> stop;
    
    mutable mutex exception_mutex;
    shared_ptr<pair<string, pair<unsigned int, unsigned int>>> exception_ptr;
};
