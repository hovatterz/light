# Let There Be Light
Let There Be Light is a 2D dynamic soft shadows system that uses SFML.

[SFML Forum Thread](http://en.sfml-dev.org/forums/index.php?topic=6635.0)

I am not the original author of this code. I modified it to use the CMake
build sytem, SFML 2.0, and Boost instead of C++0x. This has only been
tested on OS X 10.7 Lion, so please submit any issues you have compiling
on other systems.

## Compiling
Do an out of source build. Specifically:

    mkdir build && cd build
    cmake ..
    make

You can then say `bin/sample` to run the sample. I recommend using `buid/`
as your out of source build directory, because it is already on the
`.gitignore`.

## Contributing
Stick with the style of the library as much as possible. Check out a 
feature branch and make your commits. Rebase against `origin/master` and
submit your pull request.

For example:

    git checkout -b add-foo
    # make changes
    git commit -a -m "Some message"
    # fix something
    git commit -a -m "Fix some message"
    git fetch origin
    git rebase -i origin/master
    # do interactive rebase
    # submit pull request from you/add-foo to origin/master

## License
[zlib](http://www.gzip.org/zlib/zlib_license.html)
