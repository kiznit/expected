# std::expected

This is an implementation of std::expected as proposed in [P0323R12](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0323r12.html).

Follow the proposal as close as possible. Don't try to abstract things like constraints, abstractions are leaky and can be a source of error. Adding all the constraints as specified by the proposal can be very noisy, but it makes it easier to verify that the code matches what the proposal says.

Unit tests provide ~~full~~ code coverage to ensure that every method is working correctly. Hours and hours of hard work work went into writing these unit tests. This was certainly headache-inducing. That being said, I am sure they are mistakes in this code. It is very unlikely I got everything right on the first attempt. Please do log issues on GitHub and feel free to submit pull requests.

Testing is done using different versions of GCC, clang, mingw and MSVC. You can see more details on the GitHub [build page](https://github.com/kiznit/expected/actions/workflows/build.yml).


## Features

- Requires C++ 20 (some might not consider this a feature).
- Works with or without C++ exceptions enabled.
- Follows the [P0323R12](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0323r12.html) proposal as closely as possible.
- Implements the [P2505R5](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2505r5.html) proposal as well.

## namespace std

Prior to C++ 20, **std::unexpected** used to be a function. Major compilers still define it as such in their C++ libraries. Header **&lt;expected&gt;** implements a workaround using macro trickery. If you get compile-time error related to **std::unexpected** being a function, ensure that you include **&lt;expected&gt;** before you include **&lt;exception&gt;** or any other standard header that includes **&lt;exception&gt;**. To play it safe, make sure to include **&lt;expected&gt;** before any other standard header. This is known to work on GCC, clang, mingw and MSVC.

An alternative workaround is to not include **&lt;expected&gt;** and instead include **&lt;kz/expected.hpp&gt;**. You can then use namespace **kz** instead of namespace **std**. For example, **std::expected** becomes **kz::expected**.

## TODO
- Refactor/rewrite unit tests and get to 100% coverage, waiting on final spec
- Generate code coverage and publish it somewhere?
