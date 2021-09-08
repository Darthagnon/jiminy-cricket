I left something out for c-toxcore
so you can debug the library.
let me know when you're ready to modify that..
I'll just explain it now..
in the c-toxcore directory there is a file called CMakeLists.txt
you have to open that and edit it
at around line 80, you'll see this set of lines:
```
# Set standard version for compiler.
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_C_EXTENSIONS OFF)
  set(CMAKE_CXX_EXTENSIONS OFF)
```
At the end of that, add these 3 lines:
```
# add debug capability for valgrind
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
```
then go into the _build directory
and run this
```
make
sudo make install
```

## Valgrind
https://valgrind.org/downloads/

you can get it with: ```wget https://sourceware.org/pub/valgrind/valgrind-3.17.0.tar.bz2```

then you can uncompress it with ```tar -xf valgrind-3.17.0.tar.bz2```

```
cd ~/development
  wget https://sourceware.org/pub/valgrind/valgrind-3.17.0.tar.bz2
  tar -xf valgrind-3.17.0.tar.bz2
  cd valgrind-3.17.0
  ./configure
  make -j 4
  sudo make install
```