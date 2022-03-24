#include "test_runner.h"

#include <functional>
#include <string>

using namespace std;

template<typename T>
class LazyValue {
public:
    explicit LazyValue(function<T()> init) {
        called = init;
    }

    bool HasValue() const {
        return opt.has_value();
    }

    const T &Get() const {
        if(!opt.has_value()) {
            opt = called();
        }
        return opt.value();
    }

private:
    function<T()> called;
    mutable optional<T> opt;
};

void UseExample() {
    const string big_string = "Giant amounts of memory";

    LazyValue<string> lazy_string([&big_string] { return big_string; });

    ASSERT(!lazy_string.HasValue());
    ASSERT_EQUAL(lazy_string.Get(), big_string);
    ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void DoSomething(const vector<string>& v) {
    for (auto it = v.begin(); it != v.end(); ++it) {
        const string &s = *it;
    }
}

void TestInitializerIsntCalled() {
    bool called = false;

    {
        LazyValue<int> lazy_int([&called] {
            called = true;
            return 0;
        });
    }
    ASSERT(!called);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, UseExample);
    RUN_TEST(tr, TestInitializerIsntCalled);
    return 0;
}