cmake -G "MinGW Makefiles" -B "./build" -DX64_BITS=ON .
cd build
make
cp ./libdistricomp.a ../lib/libdistricomp.a
cd ..
