#include <vector>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

std::mutex mtx;

std::condition_variable cv;

void push(std::vector<int> &v2){

    for(int i =0; i< 100; i++){
        std::unique_lock<std::mutex> lock(mtx);
        v2.push_back(i);
        cv.notify_one();
    }
}

void  pop(std::vector<int> &v2, std::vector<int> &v3 ){

    for(int i =0; i< 100; i++){

        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[&](){return v2.size()>0;});
        int m = v2.back();
        v2.pop_back();
        v3.push_back(m);


    }
}

int main(){

    std::vector<int> v2;
    std::vector<int> v3;
//    std::thread t1([&](){
//        push(v2);
//    });

    std::thread t1(push,std::ref(v2));
    std::thread t2(pop,std::ref(v2),std::ref(v3));
    t1.join();
    t2.join();
    std::cout << v3.size()<<std::endl;
    for(auto i : v3) std::cout << i << " " ;
    std::cout << std::endl;
    return 0;
}