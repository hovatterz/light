# Let There Be Light
Let There Be Light is a 2D dynamic soft shadows system that uses SFML.

[SFML Forum Thread](http://en.sfml-dev.org/forums/index.php?topic=6635.0)

I am not the original author of this code. I modified it to use the CMake
build sytem and SFML 2.X. This has only been tested on OS X 10.7 Mavericks,
so please submit any issues you have compiling on other systems.

## Compiling and Installing
Do an out of source build. Specifically:

    mkdir build && cd build
    cmake ..
    make

# Ubuntu
To do an out of source build in Ubuntu, follow the steps below:

    sudo apt-get install build-essential cmake libsdl2-dev libglew-dev
    cd /path/to/source
    mkdir build && cd build
    cmake ..
    make && sudo make install    

NOTE: Only tested in Ubuntu 14.10.

## Installing
`make install`

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
