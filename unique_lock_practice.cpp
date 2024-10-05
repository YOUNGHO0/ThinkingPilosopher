#include <iostream>
using namespace std;

mutex m;
unique_lock<std::mutex> uLock(m);

//uLock.unlock();
int value = 0;
void changeVal(int m) {
    uLock.lock();
    value = m;
    uLock.unlock();
}

int main(){



    for(int i =0; i<100; i++)changeVal(i);

    return 0;
}