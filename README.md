# Docker-based test servers for network-related Qt autotests

* https://wiki.qt.io/Network_Testing
* https://www.qt.io/blog/2018/11/01/docker-based-test-servers-network-related-qt-autotests


* ../configure -developer-build -opensource -nomake examples
* make -j8
* cd qt5/qtbase/tests/auto
* qt5/qtbase/qtbase/bin/qmake
* make qmake_all
* qt5/qtbase/tests/auto/network/access/qnetworkreply
* make check
