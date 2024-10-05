#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

std::vector<bool> fork(5, false);
std::vector<std::mutex> forkMutex(5);
std::condition_variable cv;
std::mutex coutMutex;  // �ܼ� ����� ���� ���ؽ� �߰�

void eat(int personNumber) {
    std::unique_lock<std::mutex> lock(forkMutex[personNumber]);
    cv.wait(lock, [&]() { return !fork[personNumber]; });
    fork[personNumber] = true;

    int next = (personNumber + 1) % 5;
    std::unique_lock<std::mutex> lock2(forkMutex[next]);
    cv.wait(lock2, [&]() { return !fork[next]; });
    fork[next] = true;

    // �ܼ� ��� �� ����ȭ
    {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << personNumber << " eats with fork " << personNumber << " and fork " << next << std::endl;
    }

    // �Ļ� �� ��ũ�� ��������
    fork[next] = false;
    fork[personNumber] = false;

    // ��ũ�� ������ ���� �˸�
    cv.notify_all();
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(eat, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
