libcore
=======
Libcore is a library providing OS wrappers features with a small footprint designed for embedded systems.
Primary targeted for ARM, it provides:

* Coroutines tasks
* Non blocking IO using epoll
* Syslog macros
* System timers
* Filesystem browser

Language: C++
Plateforms: arm/i386/x86_64

Compiling
=========
    autoreconf -vif ; ./configure ; make #should suffice.

Testing
=======
    g++ examples/bareminimum.cpp -std=gnu++11 -lCore -I ../include -o bareminimum
    OVK_LOG_LVL=6 ./bareminimum


Contributing
============


Coding Guidelines
-----------------

We use the following [Astyle](http://astyle.sourceforge.net/) configuration:

    --style=allman
    --indent-namespaces
    --indent=spaces=2
    --indent-switches
    --indent-col1-comments
    --break-blocks
    --delete-empty-lines
    --convert-tabs
    --max-code-length=175
    --indent-preproc-define
    --indent-preproc-cond
    --indent-preproc-block
    --break-blocks
    --min-conditional-indent=0
    --unpad-paren

Submit your changes
-------------------

You are welcome to submit your code change as pull request.
