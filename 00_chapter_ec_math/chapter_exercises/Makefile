CFLAGS = $(shell pkg-config --cflags libbitcoin)
LDFLAGS = $(shell pkg-config --libs libbitcoin)

all:
		g++ -std=c++11 -c src/EC_math.cpp -o src/EC_math.o -I/include $(CFLAGS)
		g++ -std=c++11 -c src/main.cpp -o src/main.o -I/include $(CFLAGS)
		g++  -std=c++11 src/EC_math.o src/main.o -o src/main $(LDFLAGS)
