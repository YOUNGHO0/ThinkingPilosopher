#include <vector>
#include <thread>
#include <mutex>
#include <iostream>


std::mutex mtx;
std::condition_variable cv;
std::vector<int> v2;  // 공유 벡터
std::vector<int> v3;  // 출력 벡터
bool done = false; // 푸시 작업이 끝났는지 표시하는 플래그

void push() {
    for (int i = 0; i < 10; i++) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            v2.push_back(i);
            std::cout << "pushed: " << i << std::endl;
        }
        cv.notify_all(); // 모든 대기 중인 스레드에 알림
    }

    // 푸시가 끝났음을 알림
    {
        std::unique_lock<std::mutex> lock(mtx);
        done = true; // 푸시가 완료되었음을 설정
    }
    cv.notify_all(); // 모든 대기 중인 스레드에 알림
}

void pop(int thread_id) {
    while (true) {
        int value;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []() { return !v2.empty() || done; }); // 벡터에 값이 있거나 푸시가 끝났을 때까지 대기
            if (v2.empty() && done) break; // 벡터가 비어있고 푸시가 끝났다면 종료

            value = v2.back();
            v2.pop_back();
            v3.push_back(value); // 팝한 값을 저장
            std::cout << "Thread " << thread_id << " popped value: " << value << std::endl;
        }
    }
}

int main() {
    std::thread t1(push); // 푸시 스레드

    std::vector<std::thread> pop_threads;
    for (int i = 0; i < 3; i++) {
        pop_threads.emplace_back(pop, i + 1); // 3개의 팝 스레드 생성
    }

    t1.join(); // 푸시 스레드가 끝날 때까지 대기

    for (auto &t : pop_threads) {
        t.join(); // 모든 팝 스레드가 끝날 때까지 대기
    }

    // 출력 벡터의 내용을 출력
    std::cout << "Values popped: " << std::endl;
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    return 0;
}
