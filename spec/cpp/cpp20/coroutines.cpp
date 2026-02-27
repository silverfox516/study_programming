#include <iostream>
#include <coroutine>

template<typename T>
struct generator {
    struct promise_type {
        T current_value;
        auto get_return_object() { return generator{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
        auto yield_value(T value) {
            current_value = value;
            return std::suspend_always{};
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro;

    generator(handle_type h) : coro(h) {}
    ~generator() { if (coro) coro.destroy(); }

    T operator()() {
        coro.resume();
        return coro.promise().current_value;
    }
};

generator<int> range(int n) {
    for (int i = 0; i < n; ++i) {
        co_yield i;
    }
}

int main() {
    auto gen = range(5);
    for (int i = 0; i < 5; ++i) {
        std::cout << gen() << " ";
    }
    std::cout << std::endl;
    return 0;
}
