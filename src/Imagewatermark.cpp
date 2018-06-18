#include "CImg.h"
#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <boost/filesystem.hpp>

using namespace cimg_library;
using namespace std;
using namespace chrono;
using namespace boost::filesystem;

typedef unsigned char imageType;

struct Message {
	CImg<imageType>* image;
	string name;
};

string EXIT = "EXIT", COLLECTOR = "COLLECTOR", WORKER = "WORKER", EMITTER = "EMITTER";

condition_variable cvEmitter, cvWorker, cvCollector;

class Timer {
	typedef time_point<system_clock> clock;

	clock time;
	duration<double> elapsed;

	public:
		void startTime() {
			time = system_clock::now();
		}

		void stopTime(){
			elapsed = system_clock::now() - time;
		}

		double getDuration() {
			return elapsed.count();
		}
};

void sendMessage( string receiver, struct Message message, queue<Message>* queue, mutex* mux, int index ) {
	if(receiver.compare( EMITTER ) == 0) {
		unique_lock<mutex> lockEmitterMessage( *mux );

		queue->push( message );
		cvEmitter.notify_one();

		lockEmitterMessage.unlock();
	} else if(receiver.compare( WORKER ) == 0) {
		unique_lock<mutex> lockWorkerMessage( *mux );

		queue->push( message );
		cvWorker.notify_one();

		lockWorkerMessage.unlock();
	} else if(receiver.compare( COLLECTOR ) == 0) {
		unique_lock<mutex> lockCollectorMessage( *mux );

		queue->push( message );
		cvCollector.notify_one();

		lockCollectorMessage.unlock();
	}
}

void collect( queue<Message>* queue, mutex* collectorMutex ) {
	struct Message message;

	while(true) {
		unique_lock<mutex> lockCollectorMessage( *collectorMutex );

		cvCollector.wait( lockCollectorMessage, [=]{ return !queue->empty(); } );

		message = queue->front();
		queue->pop();

		lockCollectorMessage.unlock();

		if(message.name.compare( EXIT ) == 0) {
			free( message.image );
			break;
		}

		cout << "[COLLECTOR]: SALVATAGGIO IMMAGINE" << endl;
		string path = "./ImageResults/" + message.name;
		char* directory = new char[path.size() + 1];
		copy( path.begin(), path.end(), directory );
		directory[path.size()] = '\0';
		message.image->save( directory );

		free( message.image );
		free( directory );
	}


	cout << "[COLLECTOR]: STA TERMINANDO..." << endl;
}

void work( int index, CImg<imageType>& stamp, queue<Message>* workerQueue, queue<Message>* collectorQueue, mutex* workerMutex, mutex* collectorMutex ) {
	struct Message message;
	CImg<imageType>::iterator itR, itG, itB, itS;

	while(true) {
		unique_lock<mutex> lockWorkerMessage( *workerMutex );

		cvWorker.wait( lockWorkerMessage, [=]{ return !workerQueue->empty(); } );

		message = workerQueue->front();
		workerQueue->pop();

		lockWorkerMessage.unlock();

		cout << "[WORKER " << index << "]: LETTURA MESSAGGIO" << endl;
		if(message.name.compare( EXIT ) == 0) {
			cout << "[WORKER " << index << "]: STA TERMINANDO..." << endl;
			return;
		}

		CImg<imageType>* image = message.image;
		int color, offset = 0, numPixels = image->width() * image->height();
		for(itS = stamp.begin(); itS < stamp.end(); ++itS, ++offset) {
			if(*itS == 0) {
				itR = image->begin();
				advance( itR, offset );
				color += *itR;

				itG = image->begin();
				advance( itG, numPixels + offset );
				color += *itG;

				itB = image->begin();
				advance( itB, numPixels * 2 + offset );
				color += *itB;

				color /= 6;

				*itR = color;
				*itG = color;
				*itB = color;
			}
		}

		sendMessage( COLLECTOR, message, collectorQueue, collectorMutex, 0 );
	}
}

void emit( int nWorkers, CImg<imageType>& stamp, queue<Message>* emitterQueue, queue<Message>* collectorQueue, mutex* emitterMutex, mutex* collectorMutex) {
	vector<thread> threads;
	vector<queue<Message>*> workersQueues;

	vector<mutex*> workersMutexes;

	for(int i = 0; i < nWorkers; i++) {
		workersQueues.push_back( new queue<Message>() );
		workersMutexes.push_back( new mutex() );
		threads.emplace_back( work, i, ref( stamp ), workersQueues[i], collectorQueue, workersMutexes[i], collectorMutex );
		cout << "[EMITTER]: WORKER[" << i << "] GENERATO" << endl;
	}

	struct Message message;
	for(int index = 0; index < numeric_limits<int>::max(); index++) {
		unique_lock<mutex> lockEmitterMessage( *emitterMutex );

		cvEmitter.wait( lockEmitterMessage, [=]{ return !emitterQueue->empty(); } );

		message = emitterQueue->front();
		emitterQueue->pop();

		lockEmitterMessage.unlock();

		cout << "[EMITTER]: LETTURA MESSAGGIO" << endl;
		if(message.image->is_empty()) {
			break;
		}

		cout << "[EMITTER]: INVIO MESSAGGIO AGLI WORKER" << endl;
		sendMessage( WORKER, message, ref( workersQueues[index % nWorkers] ), workersMutexes[index % nWorkers], index % nWorkers );
	}

	// comunicate to all the threads that the images to be processed are finished
	cout << "[EMITTER]: TERMINE ELABORAZIONE IMMAGINI" << endl;
	message.name = EXIT;
	for(int i = 0; i < nWorkers; i++) {
		sendMessage( WORKER, message, ref( workersQueues[i] ), workersMutexes[i], i );
	}

	for(thread & t : threads) {
		t.join();
	}

	// comunicate to the collector that there are no more images to save
	sendMessage( COLLECTOR, message, collectorQueue, collectorMutex, 0 );

	cout << "[EMITTER]: STA TERMINANDO..." << endl;
}

int main( int argc, char* argv[] ) {
	queue<Message> *emitterQueue = new queue<Message>();
	queue<Message> *collectorQueue = new queue<Message>();

	mutex* emitterMutex = new mutex();
	mutex* collectorMutex = new mutex();

	int dimW = atoi( argv[2] ), dimH = atoi( argv[3] );

	// creates images and save them in a directory
	imageType black[] = { 0, 0, 0 }, white[] = { 255, 255, 255 }, gray[] = { 127, 127, 127 }, color[] = { 255, 30, 130 };
	CImg<imageType> image1( dimW, dimH, 1, 3, { 255, 30, 130 } ), image2( dimW, dimH, 1, 3, 255 ),
					image3( dimW, dimH, 1, 3, 255 ), image4( dimW, dimH, 1, 3, { 255, 30, 130 } ),
					stampImage( dimW, dimH, 1, 3, 255 );
	image1.draw_text( dimW/6, dimH/8, "Sono", black, white, 1, 103 ); image2.draw_text( dimW/6, dimH/8, "Ciao", black, white, 1, 103 );
	image3.draw_text( dimW/6, dimH/8, "Vedo", black, white, 1, 103 ); image4.draw_text( dimW/6, dimH/8, "Ecco", black, color, 1, 103 );
	stampImage.draw_rectangle( dimW/8, dimH/2, dimW*3/4, dimH*3/4, black );
	//stampImage.draw_text( dimW/6, dimH/2, "Marco", black, white, 1, 103 );
	//image1.save( "./Images/image1.jpg" ); image2.save( "./Images/image2.jpg" ); image3.save( "./Images/image3.jpg" );
	//image1.save( "./Images/image1.jpg" ); image4.save( "./Images/image4.jpg" ); stampImage.save( "./StampImage/stampImage.jpg" );

	// takes the stamp image
	CImg<imageType> stamp;
	for(auto i = directory_iterator( argv[6] ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) argv[6] + "/" + i->path().filename().string();
			if(strcmp( i->path().filename().c_str(), argv[7] ) == 0) {
				stamp.assign( imageName.c_str() );
			}
		}
	}

	Timer timeN, timeS;

	struct Message message;

	cout << "[MAIN]: INIZIATA PARTE PARALLELA" << endl;

	timeN.startTime();

	thread collector( collect, collectorQueue, collectorMutex );

	thread emitter( emit, atoi( argv[1] ), ref( stamp ), emitterQueue, collectorQueue, emitterMutex, collectorMutex );

	// takes images from the directory and send them to the emitter queue
	int mexSended = 0;
	for(int j = 0; j < atoi( argv[4] ); j++) {
		for(auto i = directory_iterator( argv[5] ); i != directory_iterator(); i++) {
			if(!is_directory( i->path() )) {
				string imageName = (string) argv[5] + "/" + i->path().filename().string();
				message.image = new CImg<imageType>( imageName.c_str() );
				message.name = i->path().filename().string();

				sendMessage( EMITTER, message, emitterQueue, emitterMutex, 0 );

				mexSended++;
			}
		}
	}

	// sends messages of "EXIT" strings to the workers when #workers > #files
	message.name = EXIT;
	for(; mexSended < atoi( argv[1] ); mexSended++) {
		sendMessage( EMITTER, message, emitterQueue, emitterMutex, 0 );
	}

	// comunicates to the emitter that there are no more images
	message.image = new CImg<imageType>();
	sendMessage( EMITTER, message, emitterQueue, emitterMutex, 0 );

	emitter.join();

	free( emitterQueue );
	free( emitterMutex );

	collector.join();

	free( collectorQueue );
	free( collectorMutex );

	timeN.stopTime();

	cout << "[MAIN]: TERMINATA PARTE PARALLELA" << endl;

	cout << "[MAIN]: INIZIATA PARTE SEQUENZIALE" << endl;

    timeS.startTime();

	CImg<imageType>::iterator itR, itG, itB, itS;
	for(int j = 0; j < atoi( argv[4] ); j++) {
		for(auto i = directory_iterator( argv[5] ); i != directory_iterator(); i++) {
			if(!is_directory( i->path() )) {
				string imageName = (string) argv[5] + "/" + i->path().filename().string();
				CImg<imageType> *image = new CImg<imageType>( imageName.c_str() );
				int color, offset = 0, numPixels = image->width() * image->height();
				for(itS = stamp.begin(); itS < stamp.end(); ++itS, ++offset) {
					if(*itS == 0) {
						itR = image->begin();
						advance( itR, offset );
						color += *itR;

						itG = image->begin();
						advance( itG, numPixels + offset );
						color += *itG;

						itB = image->begin();
						advance( itB, numPixels * 2 + offset );
						color += *itB;

						color /= 6;

						*itR = color;
						*itG = color;
						*itB = color;
					}
				}

				string path = "./ImageResults/" + i->path().filename().string();
				char* directory = new char[path.size() + 1];
				copy( path.begin(), path.end(), directory );
				directory[path.size()] = '\0';
				image->save( directory );
				free( image );
				free( directory );
			}
		}
	}


	timeS.stopTime();

	cout << "[MAIN]: TERMINATA PARTE SEQUENZIALE" << endl;

	cout << "\nSequential time = " << timeS.getDuration() << "\n"
		 << "\nParallel_N time = " << timeN.getDuration() << "\n"
		 << "\nSpeedup = " << timeS.getDuration()/timeN.getDuration() << "\n";

	return 0;
}
