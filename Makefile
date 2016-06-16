

meanpayoff: meanpayoff.cpp
	g++ -D_MP_INTERNAL -D_AMD64_ -D_USE_THREAD_LOCAL -DNDEBUG -D_EXTERNAL_RELEASE  -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -fopenmp -O3 -D _EXTERNAL_RELEASE -fomit-frame-pointer -fno-strict-aliasing -D_LINUX_ -fPIC -D_LINUX_  -o meanpayoff.o meanpayoff.cpp
	g++ -o meanpayoff  meanpayoff.o -lz3 -lpthread  -fopenmp -lrt
