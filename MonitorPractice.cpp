#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class Monitor {
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<int> shared_data;
    bool ready = false;

public:
    // 데이터를 업데이트하고 조건 변수를 알림
    void updateData(int value) {
        std::unique_lock<std::mutex> lock(mtx);
        shared_data.push(value);
        ready = true;
        cv.notify_one();  // 대기 중인 스레드에게 알림
    }

    // 데이터가 준비될 때까지 대기하고 데이터를 가져옴
    void getData() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !shared_data.empty(); });  // 큐에 데이터가 있을 때까지 대기
        std::cout << "shared data : " << shared_data.front() << std::endl;
        shared_data.pop();

        // 큐가 비어 있을 경우 ready를 false로 리셋
        if (shared_data.empty()) {
            ready = false;
        }
    }
};

int main() {
    Monitor monitor;

    // 스레드 1: 데이터를 설정
    std::thread producer([&monitor]() {
        for (int i = 0; i < 100; i++) {
            monitor.updateData(i);
        }
    });

    // 스레드 2: 데이터를 가져옴
    std::thread consumer([&monitor]() {
        for (int i = 0; i < 100; i++) {
            monitor.getData();
        }
    });

    producer.join();
    consumer.join();

    return 0;
}
