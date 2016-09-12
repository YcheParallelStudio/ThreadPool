#include <iostream>
#include <vector>
#include <chrono>

#include "ThreadPool.h"

using namespace std;

int main() {

    ThreadPool pool(4);
    vector<future<int>> results;

    for (int i = 0; i < 8; ++i) {
        results.emplace_back(
                pool.enqueue(
                        [i]() -> int {
                            cout << "hello " << i << endl;
                            this_thread::sleep_for(chrono::seconds(1));
                            cout << "world " << i << endl;
                            return i * i;
                        }
                )
        );
    }

    for (auto &result: results)
        cout << result.get() << ' ';
    cout << endl;

    return 0;
}
