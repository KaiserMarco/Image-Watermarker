CC     = g++
CFLAGS = -c -Wall -pedantic -I./ #-O3
LFLAGS = -std=c++11 -lstdc++fs -pthread -lX11
OBJ    = Timer.o Message.o Queuet.o Node.o Emitter.o Worker.o Collector.o ImageWatermark.o
TARGET = imgWtr

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $(LFLAGS) $^

ImageWatermark.o: ImageWatermark.cpp Emitter.h Worker.h Queuet.h Collector.h Timer.h
	$(CC) $(CFLAGS) ImageWatermark.cpp $(LFLAGS)

Emitter.o: Emitter.cpp Emitter.h CImg.h Message.h Node.h Timer.h
	$(CC) $(CFLAGS) Emitter.cpp $(LFLAGS)

Worker.o: Worker.cpp Worker.h Node.h Message.h
	$(CC) $(CFLAGS) Worker.cpp $(LFLAGS)

Collector.o: Collector.cpp Collector.h  Node.h Message.h Timer.h
	$(CC) $(CFLAGS) Collector.cpp $(LFLAGS)

Node.o: Node.cpp Node.h Message.h CImg.h Queuet.h
	$(CC) $(CFLAGS) Node.cpp $(LFLAGS)

Queuet.o: Queuet.cpp Queuet.h Message.h
	$(CC) $(CFLAGS) Queuet.cpp $(LFLAGS)

Message.o: Message.cpp Message.h CImg.h
	$(CC) $(CFLAGS) Message.cpp $(LFLAGS)

Timer.o: Timer.cpp Timer.h
	$(CC) $(CFLAGS) Timer.cpp $(LFLAGS)

clean:
	rm -rf *o *.gch $(TARGET)

