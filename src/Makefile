all:test echoserver
FFMPPEGFLAGE=-I../include/  ../lib/libavcodec.so ../lib/libavutil.so ../lib/libswscale.so ../lib/libx264.so ../lib/libx265.so 
OPENCVFLAGE=$(shell pkg-config opencv4 --cflags --libs)
test:
	g++ main.cpp -o test -I.  ${FFMPPEGFLAGE} ${OPENCVFLAGE}

echoserver: echoserver.cpp
	g++ -std=c++17 -Wall -g -O0 -DTARGET_LINUX=1 -I../module_sockstr/include -c echoserver.cpp 	${FFMPPEGFLAGE} ${OPENCVFLAGE}
	g++ -pthread  echoserver.o ../module_sockstr/lib/libsockstr.a /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so -o echoserver ${FFMPPEGFLAGE} ${OPENCVFLAGE}


runechoserver:
	./echoserver
runtest:
	export LD_LIBRARY_PATH=../lib:../usr/lib && ./test
clean:
	rm unitv3 decoder*  test echoserver *.o encoder*