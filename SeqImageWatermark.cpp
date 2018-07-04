#include <iostream>
#include <vector>
#include <experimental/filesystem>

#include "CImg.h"

#include "Message.h"
#include "Timer.h"

using namespace cimg_library;
using namespace iwm;

namespace fs = experimental::filesystem;

typedef unsigned char imageType;

void insertWatermark( CImg<imageType>* stamp, CImg<imageType>* image ) {
	CImg<imageType>::iterator itR, itG, itB, itS;
	int color = 0, offset = 0, numPixels = image->width() * image->height();
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
}

void saveFile( string path, CImg<imageType>* image ) {
	char* directory = new char[path.size() + 1];
	copy( path.begin(), path.end(), directory );
	directory[path.size()] = '\0';

	image->save( directory );

	delete directory;
}

int main( int argc, char* argv[] ) {
	int dimW       = atoi( argv[6] );
	int dimH       = atoi( argv[7] );
	int iterations = atoi( argv[8] );

	char* imagesDir = argv[2];
	char* saveDir   = argv[3];
	char* stampDir  = argv[4];
	char* stampName = argv[5];

	CImg<imageType>* stamp;

	Timer* timeN = new Timer();
	Timer* timeS = new Timer();

	for(auto & p : fs::directory_iterator( stampDir )) {
		if(!is_directory( p.path() )) {
			string imageName = (string) stampDir + "/" + p.path().filename().string();
			if(strcmp( p.path().filename().c_str(), stampName ) == 0) {
				stamp = new CImg<imageType>( imageName.c_str() );
				stamp->resize( dimW, dimH );
			}
		}
	}

	vector<Message*>* messages = new vector<Message*>();

    for(int j = 0; j < iterations; j++) {
    	for(auto & p : fs::directory_iterator( imagesDir )) {
			if(!is_directory( p.path() )) {
				string imageName = p.path().filename().string();
				CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );
				image->resize( dimW, dimH );

				Message* mex = new Message( image, imageName );
				messages->push_back( mex );
			}
		}
	}

    timeS->startTime();

    for(unsigned int i = 0; i < messages->size(); i++) {
		Message* message = messages->at( i );

		insertWatermark( stamp, message->getImage() );

		saveFile( message->getName(), message->getImage() );

		delete message;
    }

	delete stamp;

	timeS->stopTime();

	cout << "[MAIN]: TERMINATA PARTE SEQUENZIALE" << endl;

	cout << endl << "Sequential time = " << timeS->getDuration() << " secs" << endl;

	delete timeN;
	delete timeS;

	return 0;
}
