cmake -G "MinGW Makefiles" -B "./build" -DX64_BITS=OFF .
cd build
make
cp ./libdistricomp.a ../lib/libdistricomp.a
cd ..
