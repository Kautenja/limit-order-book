// A test file for tqdm progress bar clone.
//
//

#include <unistd.h>
#include "include/tqdm.hpp"

// the themes to demo
auto themes = {
    "default",
    "vertical",
    "basic",
    "braille",
    "braille_spin",
    "line"
};

int main() {
    int N = 1000000;
    for (auto theme : themes) {
        std::cout << theme << std::endl;
        tqdm bar(N, "it", theme);
        for (int i = 0; i < N; ++i) {
            bar.update();
            // do work
            usleep(1);
        }
    }
    return 0;
}
