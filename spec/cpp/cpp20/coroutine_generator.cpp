// Hand-written coroutine generator — C++20
// What: a minimal Generator<T> built on the C++20 coroutine machinery, demonstrating
//       co_yield and the promise_type protocol.
// Why:  std::generator is C++23. Until your toolchain supports it, this is how you
//       roll your own. Even after, knowing the machinery clarifies what coroutines do.
// Pre:  no language-level coroutines; iterator+state-machine classes by hand.

#include <iostream>
#include <coroutine>
#include <utility>

template <typename T>
struct Generator {
    struct promise_type {
        T value;

        Generator      get_return_object() { return Generator{handle::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T v) { value = std::move(v); return {}; }
        void                return_void() {}
        void                unhandled_exception() { std::terminate(); }
    };

    using handle = std::coroutine_handle<promise_type>;

    explicit Generator(handle h) : h_(h) {}
    Generator(const Generator&)            = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& o) noexcept : h_(std::exchange(o.h_, {})) {}
    ~Generator() { if (h_) h_.destroy(); }

    bool next() {
        if (!h_ || h_.done()) return false;
        h_.resume();
        return !h_.done();
    }
    T value() const { return h_.promise().value; }

private:
    handle h_;
};

Generator<int> first_n_squares(int n) {
    for (int i = 1; i <= n; ++i) {
        co_yield i * i;
    }
}

Generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    auto g = first_n_squares(5);
    while (g.next()) std::cout << g.value() << " ";
    std::cout << "\n";

    auto fib = fibonacci();
    for (int i = 0; i < 10 && fib.next(); ++i) {
        std::cout << fib.value() << " ";
    }
    std::cout << "\n";

    return 0;
}
