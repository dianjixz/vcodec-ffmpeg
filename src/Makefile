target: main.cpp
	g++ main.cpp -I ../include/ -L ../lib/ -lavcodec -lavutil -lswscale -o test
run:
	export LD_LIBRARY_PATH=../lib && ./test
clean:
	rm test decoder* encoder*