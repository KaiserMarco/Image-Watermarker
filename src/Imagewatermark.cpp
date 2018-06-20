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

string EXIT_EMITTER = "EXIT EMITTER", EXIT_WORKER = "EXIT WORKER", EXIT_COLLECTOR = "EXIT COLLECTOR",
		EMITTER = "EMITTER", WORKER = "WORKER", COLLECTOR = "COLLECTOR";

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

void sendMessage( string receiver, Message& message, queue<Message>* queue, mutex* mux, condition_variable* cv ) {
	if(receiver.compare( EMITTER ) == 0) {
		unique_lock<mutex> lockEmitterMessage( *mux );

		queue->push( ref( message ) );
		cv->notify_one();

		lockEmitterMessage.unlock();
	} else if(receiver.compare( WORKER ) == 0) {
		unique_lock<mutex> lockWorkerMessage( *mux );

		queue->push( ref( message ) );
		cv->notify_one();

		lockWorkerMessage.unlock();
	} else if(receiver.compare( COLLECTOR ) == 0) {
		unique_lock<mutex> lockCollectorMessage( *mux );

		queue->push( ref( message ) );
		cv->notify_one();

		lockCollectorMessage.unlock();
	}
}

void collect( string dir, queue<Message>* queue, mutex* collectorMutex, condition_variable* cvCollector ) {
	while(true) {
		unique_lock<mutex> lockCollectorMessage( *collectorMutex );

		cvCollector->wait( lockCollectorMessage, [=]{ return !queue->empty(); } );

		Message& message = queue->front();
		queue->pop();

		lockCollectorMessage.unlock();

		if(message.name.compare( EXIT_COLLECTOR ) == 0) {
			break;
		}

		cout << "[COLLECTOR]: SALVATAGGIO IMMAGINE" << endl;
		string path = dir  + string( "/" ) + message.name;
		char* directory = new char[path.size() + 1];
		copy( path.begin(), path.end(), directory );
		directory[path.size()] = '\0';
		message.image->save( directory );

		delete message.image;
		delete directory;
	}


	cout << "[COLLECTOR]: STA TERMINANDO..." << endl;
}

void work( int index, CImg<imageType>* stamp, queue<Message>* workerQueue, queue<Message>* collectorQueue,
		   mutex* workerMutex, mutex* collectorMutex, condition_variable* cvWorker, condition_variable* cvCollector ) {

	while(true) {
		unique_lock<mutex> lockWorkerMessage( *workerMutex );

		cvWorker->wait( lockWorkerMessage, [=]{ return !workerQueue->empty(); } );

		Message& message = workerQueue->front();
		workerQueue->pop();

		lockWorkerMessage.unlock();

		cout << "[WORKER " << index << "]: LETTURA MESSAGGIO" << endl;
		if(message.name.compare( EXIT_WORKER ) == 0) {
			cout << "[WORKER " << index << "]: STA TERMINANDO..." << endl;
			return;
		}

		CImg<imageType>::iterator itR, itG, itB, itS;
		CImg<imageType>* image = message.image;
		int color, offset = 0, numPixels = image->width() * image->height();
		for(itS = stamp->begin(); itS < stamp->end(); ++itS, ++offset) {
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

		sendMessage( COLLECTOR, ref( message ), collectorQueue, collectorMutex, cvCollector );
	}
}

void emit( int nWorkers, CImg<imageType>* stamp,
		   queue<Message>* emitterQueue, vector<queue<Message>*> workersQueues, queue<Message>* collectorQueue,
		   mutex* emitterMutex, vector<mutex*> workersMutexes, mutex* collectorMutex,
		   condition_variable* cvEmitter, vector<condition_variable*> cvWorkers, condition_variable* cvCollector ) {

	vector<thread> threads;

	for(int i = 0; i < nWorkers; i++) {
		workersQueues.push_back( new queue<Message>() );
		workersMutexes.push_back( new mutex() );
		cvWorkers.push_back( new condition_variable() );
		threads.emplace_back( work, i, stamp, workersQueues[i], collectorQueue, workersMutexes[i], collectorMutex, cvWorkers[i], cvCollector );
		cout << "[EMITTER]: WORKER[" << i << "] GENERATO" << endl;
	}

	for(int index = 0; index < numeric_limits<int>::max(); index++) {
		unique_lock<mutex> lockEmitterMessage( *emitterMutex );

		cvEmitter->wait( lockEmitterMessage, [=]{ return !emitterQueue->empty(); } );

		Message& message = emitterQueue->front();
		emitterQueue->pop();

		lockEmitterMessage.unlock();

		cout << "[EMITTER]: LETTURA MESSAGGIO" << endl;
		if(message.name.compare( EXIT_EMITTER ) == 0) {
			break;
		}

		cout << "[EMITTER]: INVIO MESSAGGIO AGLI WORKER" << endl;
		int idx = index % nWorkers;
		sendMessage( WORKER, ref( message ), workersQueues[idx], workersMutexes[idx], cvWorkers[idx] );
	}

	// comunicate to all the threads that the images to be processed are finished
	cout << "[EMITTER]: TERMINE ELABORAZIONE IMMAGINI" << endl;
	Message message;
	message.name = EXIT_WORKER;
	for(int i = 0; i < nWorkers; i++) {
		sendMessage( WORKER, ref( message ), workersQueues[i], workersMutexes[i], cvWorkers[i] );
	}

	for(thread & t : threads) {
		t.join();
	}

	// comunicate to the collector that there are no more images to save
	message.name = EXIT_COLLECTOR;
	sendMessage( COLLECTOR, ref( message ), collectorQueue, collectorMutex, cvCollector );

	cout << "[EMITTER]: STA TERMINANDO..." << endl;
}

int main( int argc, char* argv[] ) {
	Timer timeN, timeS;

	cout << "[MAIN]: INIZIATA PARTE PARALLELA" << endl;

	timeN.startTime();

	queue<Message> *emitterQueue = new queue<Message>();
	vector<queue<Message>*> workersQueues;
	queue<Message> *collectorQueue = new queue<Message>();

	mutex *emitterMutex = new mutex();
	vector<mutex*> workersMutexes;
	mutex *collectorMutex = new mutex();

	condition_variable *cvEmitter = new condition_variable();
	vector<condition_variable*> cvWorkers;
	condition_variable *cvCollector = new condition_variable();

	// takes stamp and images from the directories and send them to the emitter queue
	for(auto i = directory_iterator( argv[4] ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) argv[4] + "/" + i->path().filename().string();
			if(strcmp( i->path().filename().c_str(), argv[5] ) == 0) {
				CImg<imageType>* stamp = new CImg<imageType>( imageName.c_str() );

				thread collector( collect, argv[3], collectorQueue, collectorMutex, cvCollector );

				thread emitter( emit, atoi( argv[1] ), stamp, emitterQueue, workersQueues, collectorQueue,
								emitterMutex, workersMutexes, collectorMutex, cvEmitter, cvWorkers, cvCollector );

				int mexSent = 0;
				for(int j = 0; j < atoi( argv[8] ); j++) {
					for(auto i = directory_iterator( argv[2] ); i != directory_iterator(); i++) {
						if(!is_directory( i->path() )) {
							string imageName = (string) argv[2] + "/" + i->path().filename().string();
							Message message;
							message.image = new CImg<imageType>( imageName.c_str() );
							message.name = i->path().filename().string();

							sendMessage( EMITTER, ref( message ), emitterQueue, emitterMutex, cvEmitter );

							mexSent++;
						}
					}
				}

				// sends "EXIT" messages to the workers when #workers > #files
				Message message;
				message.name = EXIT_WORKER;
				for(; mexSent < atoi( argv[1] ); mexSent++) {
					sendMessage( EMITTER, ref( message ), emitterQueue, emitterMutex, cvEmitter );
				}

				// comunicates to the emitter that there are no more images
				message.name = EXIT_EMITTER;
				sendMessage( EMITTER, ref( message ), emitterQueue, emitterMutex, cvEmitter );

				emitter.join();

				collector.join();

				delete stamp;

				break;
			}
		}
	}

	timeN.stopTime();

	cout << "[MAIN]: TERMINATA PARTE PARALLELA" << endl;

	cout << "[MAIN]: INIZIATA PARTE SEQUENZIALE" << endl;

    timeS.startTime();

	for(auto i = directory_iterator( argv[4] ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) argv[4] + "/" + i->path().filename().string();
			if(strcmp( i->path().filename().c_str(), argv[5] ) == 0) {
				CImg<imageType>* stamp = new CImg<imageType>( imageName.c_str() );

				for(int j = 0; j < atoi( argv[8] ); j++) {
					for(auto i = directory_iterator( argv[2] ); i != directory_iterator(); i++) {
						if(!is_directory( i->path() )) {
							string imageName = (string) argv[2] + "/" + i->path().filename().string();
							CImg<imageType> *image = new CImg<imageType>( imageName.c_str() );
							int color, offset = 0, numPixels = image->width() * image->height();
							CImg<imageType>::iterator itR, itG, itB, itS;
							for(itS = stamp->begin(); itS < stamp->end(); ++itS, ++offset) {
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

							string path = argv[3] + string( "/" ) + i->path().filename().string();
							char* directory = new char[path.size() + 1];
							copy( path.begin(), path.end(), directory );
							directory[path.size()] = '\0';
							image->save( directory );
							delete image;
							delete directory;
						}
					}
				}

				delete stamp;
				break;
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
