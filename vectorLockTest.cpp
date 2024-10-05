#include <vector>
#include <thread>

void push(std::vector<int> &v2){

    for(int i =0; i< 10000; i++){
        v2.push_back(i);
    }
}

void  pop(std::vector<int> &v2, std::vector<int> &v3 ){

    for(int i =0; i< 10000; i++){
        if(!v2.empty()){
            v2.pop_back();

        }

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


    return 0;

}