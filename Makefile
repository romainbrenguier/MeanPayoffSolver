discountedsum: discountedsum.cpp
	g++ -D_MP_INTERNAL -D_AMD64_ -D_USE_THREAD_LOCAL -DNDEBUG -D_EXTERNAL_RELEASE  -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -D _EXTERNAL_RELEASE -fomit-frame-pointer -fno-strict-aliasing -fPIC -I/usr/local/include  -o discountedsum.o discountedsum.cpp
	g++ -o discountedsum discountedsum.o -L/usr/local/lib -lz3 -lpthread

meanpayoff: meanpayoff.cpp
	g++ -D_MP_INTERNAL -D_AMD64_ -D_USE_THREAD_LOCAL -DNDEBUG -D_EXTERNAL_RELEASE  -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -D _EXTERNAL_RELEASE -fomit-frame-pointer -fno-strict-aliasing -fPIC -I/usr/local/include  -o meanpayoff.o meanpayoff.cpp
	g++ -o meanpayoff  meanpayoff.o -L/usr/local/lib -lz3 -lpthread
