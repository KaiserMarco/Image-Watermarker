CC = g++
CFLAGS = -std=c++11 -Wall -pedantic -O3 -c
LFLAGS = -lstdc++fs -pthread -lX11
OBJ = ImageWatermark.o Emitter.o Worker.o Collector.o Message.o Queuet.o Timer.o
TARGET = imgWtr

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LFLAGS)

ImageWatermark.o: ImageWatermark.cpp Emitter.h Worker.h Collector.h Timer.h
	$(CC) $(CFLAGS) ImageWatermark.cpp $(LFLAGS)

Emitter.o: Emitter.h Emitter.cpp Message.h Node.h Timer.h
	$(CC) $(CFLAGS) Emitter.cpp $(LFLAGS)

Worker.o: Worker.h Worker.cpp Node.h Message.h
	$(CC) $(CFLAGS) Worker.cpp $(LFLAGS)

Collector.o: Collector.h Collector.cpp Node.h Message.h Timer.h
	$(CC) $(CFLAGS) Collector.cpp $(LFLAGS)

Queuet.o: Queuet.h Queuet.cpp Message.h
	$(CC) $(CFLAGS) Queuet.cpp $(LFLAGS)

Message.o: Message.h Message.cpp CImg.h
	$(CC) $(CFLAGS) Message.cpp $(LFLAGS)

Timer.o: Timer.h Timer.cpp
	$(CC) $(CFLAGS) Timer.cpp $(LFLAGS)

clean:
	rm -rf *o $(TARGET)