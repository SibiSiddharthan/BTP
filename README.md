# BTP
This is my first GitHub repository.

Requirements
1) compiler with C++17 support
2) cmake 3.13 or higher
3) Python3
4) Perl


Build Steps
1) Clone repository : git clone --recurse-submodules https://www.github.com/SibiSiddharthan/BTP.git
2) Install dependencies : ipython install-dependencies.py [generator] . Note for windows use visual studio or mingw.
3) Build : cmake -S . -B ./build -G [generator]