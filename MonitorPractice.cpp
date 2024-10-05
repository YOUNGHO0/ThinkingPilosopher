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
    // �����͸� ������Ʈ�ϰ� ���� ������ �˸�
    void updateData(int value) {
        std::unique_lock<std::mutex> lock(mtx);
        shared_data.push(value);
        ready = true;
        cv.notify_one();  // ��� ���� �����忡�� �˸�
    }

    // �����Ͱ� �غ�� ������ ����ϰ� �����͸� ������
    void getData() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !shared_data.empty(); });  // ť�� �����Ͱ� ���� ������ ���
        std::cout << "shared data : " << shared_data.front() << std::endl;
        shared_data.pop();

        // ť�� ��� ���� ��� ready�� false�� ����
        if (shared_data.empty()) {
            ready = false;
        }
    }
};

int main() {
    Monitor monitor;

    // ������ 1: �����͸� ����
    std::thread producer([&monitor]() {
        for (int i = 0; i < 100; i++) {
            monitor.updateData(i);
        }
    });

    // ������ 2: �����͸� ������
    std::thread consumer([&monitor]() {
        for (int i = 0; i < 100; i++) {
            monitor.getData();
        }
    });

    producer.join();
    consumer.join();

    return 0;
}
