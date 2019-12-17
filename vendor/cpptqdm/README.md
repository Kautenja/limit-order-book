# cpptqdm

A clone of the Python `tqdm` package in C++.

## Usage

```cpp
#include "tqdm.hpp"

tqdm bar(N);
for (int i = 0; i < N; ++i) {
    bar.update();
    // do work
}
```

## Aesthetic

By default, the progress bar matches the theme of `tqdm`.

```
20%|████████                                | 200288/1000000 [00:00<00:03, 233793it/s]
```

## Demo

To demonstrate the progress bar (and all themes), run the following:

```shell
g++ test.cpp -std=c++11 && ./a.out
```

## FAQ

### Won't this slow down my loops?

If your loops are faster than ~200MHz, then maybe!
