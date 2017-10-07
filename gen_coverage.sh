mkdir build &&
cd build &&
# Remove build files
rm -rf CMakeFiles/ &&
rm -rf coverage/ &&
rm -f atomical-test cmake_install.cmake CMakeCache.txt libatomical.a Makefile &&
# Make and run
cmake ../. && 
make &&
./atomical-test &&
# Generate coverage files
mkdir coverage &&
find . -name "test.cpp.gcda" -exec mv {} ./coverage \; &&
find . -name "test.cpp.gcno" -exec mv {} ./coverage \; &&
lcov --directory ./coverage --capture --output-file ./coverage/coverage.info &&
lcov --remove ./coverage/coverage.info '/usr/*' '*_test.cpp' '*doctest.h' --output-file ./coverage/coverage.info &&
lcov --list ./coverage/coverage.info