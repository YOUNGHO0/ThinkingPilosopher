#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::vector<bool> fork(5, false);
std::vector<std::mutex> forkMutex(5);
std::condition_variable cv;
std::mutex coutMutex;  // �ܼ� ����� ���� ���ؽ� �߰�

void eat(int personNumber) {
    // ù ��° ��ũ �õ�
    if (!forkMutex[personNumber].try_lock()) {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << personNumber << " failed to lock fork " << personNumber << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ��� �� �ٽ� �õ�
        return; // ���� �Ļ� �ߴ�
    }

    // �� ��° ��ũ �õ�
    int next = (personNumber + 1) % 5;
    if (!forkMutex[next].try_lock()) {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << personNumber << " failed to lock fork " << next << std::endl;
        forkMutex[personNumber].unlock(); // ù ��° ��ũ�� ��������
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ��� �� �ٽ� �õ�
        return; // ���� �Ļ� �ߴ�
    }

    // �ܼ� ��� �� ����ȭ
    {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << personNumber << " eats with fork " << personNumber << " and fork " << next << std::endl;
    }

    // �Ļ� �� ��ũ�� ��������
    forkMutex[next].unlock();
    forkMutex[personNumber].unlock();
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
