cd build && 
make clean &&
cmake CMakeLists.txt && 
make && 
./atomical-test -ts="Ast" -s