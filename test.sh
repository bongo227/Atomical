cd build && 
make clean &&
cmake CMakeLists.txt && 
make && 
./atomical-test -tc="functions" -s