# std::expected

This is an implementation of std::expected as proposed in [P0323R12](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0323r12.html).

Follow the proposal as close as possible. Don't try to abstract things like constraints, abstractions are leaky and can be a source of error. Adding all the constraints as specified by the proposal can be very noisy, but it makes it easier to verify that the code matches what the proposal says.

Unit tests provide full code coverage to ensure that every method is working correctly. Hours and hours of hard work work went into writing these unit tests. This was certainly headache-inducing. That being said, I am sure they are mistakes in this code. It is very unlikely I got everything right on the first attempt. Please do log issues on GitHub and feel free to submit pull requests.

Testing is done using different versions of GCC, clang, mingw and MSVC. You can see more details on the GitHub [build page](https://github.com/kiznit/expected/actions/workflows/build.yml).


## Features

- Requires C++ 20 (some might not consider this a feature).
- Works with or without C++ exceptions enabled.
- Follows the [P0323R12](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0323r12.html) proposal as closely as possible.

## namespace std

The current proposal for expected, [P0323R12](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0323r12.html), specifies that all the code should reside in namespace std. I would love to do so, but unfortunately **std::unexpected** conflicts with a deprecated function with the same name on multiple compiles (GCC 10-13, clang 11-13, mingw 10-11). For these reasons it is simply not possible to do so eveywhere when exceptions are enabled. If you still want to use **std::except** instead of **kz::except**, you can do so by including **<expected>**. Everything that can be imported into the **std** namespace will be. `**std::unexpected** will become **std::unexpected_** where the conflict exists.

## TODO
- Refactor/rewrite unit tests and get to 100% coverage, waiting on final spec
- Generate code coverage and publish it somewhere?
