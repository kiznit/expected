# std::expected

This is an implementation of std::expected as proposed in [P0323R10](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0323r10.html).

The code currently requires C++20, but the plan is to make this work on C++17 and possibly C++14. Work with and without exceptions handling enabled.

Follow the proposal as close as possible. Don't try to abstract things like constraints, abstractions are leaky and can be a source of error. Adding all the constraints as specified by the proposal can be very noisy, but it makes it easier to verify that the code matches what the proposal says.

Unit tests provide full code coverage to ensure that every method is working correctly. Hours and hours of hard work work went into writing these unit tests. This was certainly headache-inducing. That being said, I am sure they are mistakes in this code. It is very unlikely I got everything right on the first attempt. Please do log issues on GitHub and feel free to submit pull requests.

These units test are not perfect. I've learned a great deal writing them and if I was to redo it, I could most likely do a better job. But so much time was spent here that I am not sure I want to do it again. The value of doing so to catch errors is not obvious, but it would certainly make the code more readable.

Testing is done using different versions of GCC, clang, mingw and MSVC. You can see more details on the GitHub [build page](https://github.com/kiznit/expected/actions/workflows/build.yml).


## Features

- Support for disabled exception handling (-fno-exceptions)
- Follows the [P0323R10](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0323r10.html) proposal as closely as possible.

## namespace mtl

As part of the development efforts for Rainbow OS, I am developing a freestanding library for bare metal targets. This library is named "metal" to allude to bare metal and my love of heavy metal music. As part of metal, I was working on adding support for *expected* and decided it might be worth splitting it out into its own project. The namespace was original also named "metal" but I found that name to be a bit too long in practice. This is when I settled for "mtl" which happens to also be the abbreviation of the city I was born in: Montréal.

The current proposal for expected, [P0323R10](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0323r10.html), specifies that all the code should reside in namespace std. I would love to do so, but unfortunately **std::unexpected** conflicts with a deprecated function with the same name on multiple compiles (GCC 10-13, clang 11-13, mingw 10-11). For these reasons it is simply not possible to do so eveywhere *when exceptions are enabled*. If you still want to use **std::except** instead of **mtl::except**, you can do so by including **<expected>**. Everything that can be imported into the **std** namespace will be. `**std::unexpected** will become **std::unexpected_** where the conflict exists.

## Is this library ready for use?

Yes it is. Although it is labelled as "preview", it is fully functional and used in some of my other projects. I intent on addressing a number of items on the TODO list below before creating a non-preview release.

## TODO
- Support C++17 and even C++14?
- Generate code coverage and publish it somewhere?
- Revise all constraints, use C++20 constraints?
- Add noexcept to all relevant methods
- Do we need a const version of expected_storage?
- Are we handling T=const/volatile void properly?
- Add static_assert<>s at top of expected/unexpected to enforce certain constraints?
- Refactor/rewrite unit tests and get to 100% coverage
