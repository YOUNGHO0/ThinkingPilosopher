#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::vector<std::mutex> forkMutex(5);
std::condition_variable cv;
std::mutex coutMutex;  // 콘솔 출력을 위한 뮤텍스 추가

void eat(int personNumber) {
    int left = personNumber;
    int right = (personNumber + 1) % 5;

    while (true) {
        // 비대칭적인 포크 획득 순서: 짝수는 오른쪽 먼저, 홀수는 왼쪽 먼저
        if (personNumber % 2 == 0) {
            std::swap(left, right);
        }

        // 락 시도
        std::unique_lock<std::mutex> leftLock(forkMutex[left], std::defer_lock);
        std::unique_lock<std::mutex> rightLock(forkMutex[right], std::defer_lock);

        // 두 포크 모두를 락 시도
        if (std::try_lock(leftLock, rightLock) == -1) {
            // 락 획득 성공 -> 식사 시작
            {
                std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << personNumber << " eats with fork " << left << " and fork " << right << std::endl;
            }

            break;  // 식사 완료 후 루프 탈출
        } else {
            // 락 획득 실패 -> 실패 메시지 출력 후 재시도
            {
                std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << personNumber << " failed to lock fork " << left << " or fork " << right << std::endl;
            }

            // 잠시 대기 후 재시도
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
