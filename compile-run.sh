clang++ -o eva-llvm $(llvm-config --cxxflags --ldflags --system-libs --libs core) eva-llvm.cpp

#run main
./eva-llvm

#execute generated IR
lli ./out.ll
