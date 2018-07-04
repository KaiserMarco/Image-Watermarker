CC = g++
CFLAGS = -std=c++17 -Wall -pedantic -O3
LFLAGS = -lstdc++fs -pthread -L/home/marco/Scrivania/CImg-2.2.3 -lX11
OBJ = ImageWatermark.o Node.o Emitter.o Worker.o Collector.o Message.o Queuet.o Timer.o
TARGET = imageWatermark

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o imgWtr

ImageWatermark.o: ImageWatermark.cpp Emitter.h Worker.h Collector.h Queuet.h Timer.h
	$(CC) $(CFLAGS) $(LFLAGS) ImageWatermark.cpp

Node.o: Node.cpp Node.h Queuet.h
	$(CC) $(CFLAGS) $(LFLAGS) Node.cpp

Emitter.o: Emitter.cpp Emitter.h Message.h Node.h Timer.h
	$(CC) $(CFLAGS) $(LFLAGS) Emitter.cpp

Worker.o: Worker.cpp Worker.h Node.h Message.h
	$(CC) $(CFLAGS) $(LFLAGS) Worker.cpp

Collector.o: Collector.cpp Collector.h Node.h Message.h Timer.h
	$(CC) $(CFLAGS) $(LFLAGS) Collector.cpp

Message.o: Message.cpp CImg.h Message.h
	$(CC) $(CFLAGS) $(LFLAGS) Message.cpp

Queuet.o: Queuet.cpp Queuet.h Message.h
	$(CC) $(CFLAGS) $(LFLAGS) Queuet.cpp

Timer.o: Timer.cpp Timer.h
	$(CC) $(CFLAGS) $(LFLAGS) Timer.cpp

clean:
	rm -rf *o app