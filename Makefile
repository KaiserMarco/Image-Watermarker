CC = g++
CFLAGS = -std=c++11 -Wall -pedantic -O3
LFLAGS = -lstdc++fs -pthread -lX11
FFFLAGS = -DNO_DEFAULT_MAPPING -I $(FF_HOME) -faligned-new

seq:
	$(CC) $(CFLAGS) -o seqImgWtr SeqImageWatermark.cpp Message.cpp Timer.cpp $(LFLAGS)

par:
	$(CC) $(CFLAGS) -o parImgWtr ImageWatermark.cpp Emitter.cpp Worker.cpp Collector.cpp Node.cpp Queuet.cpp Message.cpp Timer.cpp $(LFLAGS)

ffc:
	$(CC) $(CFLAGS) $(FFFLAGS) -o ffImgWtr FfImageWatermark.cpp Message_ff.cpp $(LFLAGS)

clean:
	rm -rf *o app
