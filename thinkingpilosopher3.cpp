#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::vector<std::mutex> forkMutex(5);
std::condition_variable cv;
std::mutex coutMutex;  // �ܼ� ����� ���� ���ؽ� �߰�

void eat(int personNumber) {
    int left = personNumber;
    int right = (personNumber + 1) % 5;

    while (true) {
        // ���Ī���� ��ũ ȹ�� ����: ¦���� ������ ����, Ȧ���� ���� ����
        if (personNumber % 2 == 0) {
            std::swap(left, right);
        }

        // �� �õ�
        std::unique_lock<std::mutex> leftLock(forkMutex[left], std::defer_lock);
        std::unique_lock<std::mutex> rightLock(forkMutex[right], std::defer_lock);

        // �� ��ũ ��θ� �� �õ�
        if (std::try_lock(leftLock, rightLock) == -1) {
            // �� ȹ�� ���� -> �Ļ� ����
            {
                std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << personNumber << " eats with fork " << left << " and fork " << right << std::endl;
            }

            break;  // �Ļ� �Ϸ� �� ���� Ż��
        } else {
            // �� ȹ�� ���� -> ���� �޽��� ��� �� ��õ�
            {
                std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << personNumber << " failed to lock fork " << left << " or fork " << right << std::endl;
            }

            // ��� ��� �� ��õ�
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
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
