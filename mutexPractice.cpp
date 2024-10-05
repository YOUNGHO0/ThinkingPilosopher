#include <mutex>
#include <thread>
#include <iostream>

using namespace std;

mutex mtx; // ���ؽ� ����
int counter = 0;
int counter2 = 0;

void increaseCountWithLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        mtx.lock(); // ���ؽ� ���
        ++counter;  // �����ϰ� counter ����
        mtx.unlock(); // ���ؽ� ��� ����
    }
}

void decreaseCountWithLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        mtx.lock(); // ���ؽ� ���
        --counter;  // �����ϰ� counter ����
        mtx.unlock(); // ���ؽ� ��� ����
    }
}

void increaseCountWithoutLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        ++counter2; // �������� �ʰ� counter2 ����
    }
}

void decreaseCountWithoutLock(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        --counter2; // �������� �ʰ� counter2 ����
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
    cout << "with Lock: " << counter << endl;    // counter ���
    cout << "without Lock: " << counter2 << endl; // counter2 ���

    return 0;
}
