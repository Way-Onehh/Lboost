//用户态 有栈 对称 串行 单向 协程
//兼容性好 灵活性好 开销大 stackful

#include <iostream>
#include <boost/coroutine2/all.hpp>
template<typename T>
using coroutine = boost::coroutines2::coroutine<T>;
void foo(coroutine<int>::pull_type & sink){
    auto res = sink.get();
    std::cout<<"res: "<<res<<std::endl;
    sink();

    for (int i = 0; i < 10; i++) {
        std::cout << "retrieve " << sink.get() << "\n";
        sink();
    }

    for(auto i : sink)
        std::cout<<"retrieve "<<i<<std::endl;

    for(auto it = begin(sink); it != end(sink); ++it)
        std::cout << "retrieve "<<*it << "\n";
}
auto main() -> int {
    coroutine<int>::push_type source(foo);
    source(1);
    for(int i = 0; i < 10; i++){
        source(i);
    }

    for(int i = 0; i < 10; i++){
        source(i);
    }

    for(int i = 0; i < 10; i++){
        source(i);
    }
    return 0;
}