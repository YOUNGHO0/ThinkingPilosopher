#include <mutex>
#include <thread>
#include <iostream>

using namespace std;

mutex mtx; // 뮤텍스 선언
int counter = 0;
int counter2 = 0;

void increaseCountWithLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        mtx.lock(); // 뮤텍스 잠금
        ++counter;  // 안전하게 counter 증가
        mtx.unlock(); // 뮤텍스 잠금 해제
    }
}

void decreaseCountWithLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        mtx.lock(); // 뮤텍스 잠금
        --counter;  // 안전하게 counter 증가
        mtx.unlock(); // 뮤텍스 잠금 해제
    }
}

void increaseCountWithoutLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        ++counter2; // 안전하지 않게 counter2 증가
    }
}

void decreaseCountWithoutLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        --counter2; // 안전하지 않게 counter2 증가
    }
}
int main() {
    const int num_iterations = 10000;

    thread t1(decreaseCountWithLock, num_iterations);
    thread t2(increaseCountWithLock, num_iterations);
    thread t3(increaseCountWithoutLock, num_iterations);
    thread t4(decreaseCountWithoutLock, num_iterations);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    cout << "with Lock: " << counter << endl;    // counter 출력
    cout << "without Lock: " << counter2 << endl; // counter2 출력

    return 0;
}
