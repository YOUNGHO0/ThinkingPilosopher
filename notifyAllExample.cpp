#include <vector>
#include <thread>
#include <mutex>
#include <iostream>


std::mutex mtx;
std::condition_variable cv;
std::vector<int> v2;  // ���� ����
std::vector<int> v3;  // ��� ����
bool done = false; // Ǫ�� �۾��� �������� ǥ���ϴ� �÷���

void push() {
    for (int i = 0; i < 10; i++) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            v2.push_back(i);
            std::cout << "pushed: " << i << std::endl;
        }
        cv.notify_all(); // ��� ��� ���� �����忡 �˸�
    }

    // Ǫ�ð� �������� �˸�
    {
        std::unique_lock<std::mutex> lock(mtx);
        done = true; // Ǫ�ð� �Ϸ�Ǿ����� ����
    }
    cv.notify_all(); // ��� ��� ���� �����忡 �˸�
}

void pop(int thread_id) {
    while (true) {
        int value;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []() { return !v2.empty() || done; }); // ���Ϳ� ���� �ְų� Ǫ�ð� ������ ������ ���
            if (v2.empty() && done) break; // ���Ͱ� ����ְ� Ǫ�ð� �����ٸ� ����

            value = v2.back();
            v2.pop_back();
            v3.push_back(value); // ���� ���� ����
            std::cout << "Thread " << thread_id << " popped value: " << value << std::endl;
        }
    }
}

int main() {
    std::thread t1(push); // Ǫ�� ������

    std::vector<std::thread> pop_threads;
    for (int i = 0; i < 3; i++) {
        pop_threads.emplace_back(pop, i + 1); // 3���� �� ������ ����
    }

    t1.join(); // Ǫ�� �����尡 ���� ������ ���

    for (auto &t : pop_threads) {
        t.join(); // ��� �� �����尡 ���� ������ ���
    }

    // ��� ������ ������ ���
    std::cout << "Values popped: " << std::endl;
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    return 0;
}
