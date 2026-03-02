# C compiler
CC = /home/ilgeco//bin/llvm14/bin/clang++
CC_FLAGS = -g -fopenmp  -O2 -DOPEN


backprop: backprop.o facetrain.o imagenet.o backprop_kernel.o 
	$(CC) $(CC_FLAGS) backprop.o facetrain.o imagenet.o backprop_kernel.o libkonrolib.a -o backprop -lm -ldlb

%.o: %.[ch]
	$(CC) $(CC_FLAGS) $< -c

facetrain.o: facetrain.cpp backprop.h
	$(CC) $(CC_FLAGS) facetrain.cpp -c
	
backprop.o: backprop.cpp backprop.h
	$(CC) $(CC_FLAGS) backprop.cpp -c

backprop_kernel.o: backprop_kernel.cpp backprop.h
	$(CC) $(CC_FLAGS) backprop_kernel.cpp -c

imagenet.o: imagenet.cpp backprop.h
	$(CC) $(CC_FLAGS) imagenet.cpp -c


clean:
	rm -f *.o *~ backprop backprop_cuda.linkinfo
