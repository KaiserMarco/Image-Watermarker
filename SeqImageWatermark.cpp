#include <iostream>
#include <experimental/filesystem>
#include <fstream>

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

		if(*itS == 0) {
			*itR = color;
			*itG = color;
			*itB = color;
		}
	}
}

void saveFile( string dir, Message* message ) {
	string path = dir + message->getName();
	cout << "Saving: " << path << endl;
	char* directory = new char[path.size() + 1];
	copy( path.begin(), path.end(), directory );
	directory[path.size()] = '\0';

	message->getImage()->save( directory );

	delete[] directory;
}

int main( int argc, char* argv[] ) {
	if(argc != 10) {
		cout << "arg 1 = directory where to find the images" << endl;
		cout << "arg 2 = directory where to save the images" << endl;
		cout << "arg 3 = directory where to find the stamp image" << endl;
		cout << "arg 4 = name of the stamp image (.jpg)" << endl;
		cout << "arg 5 = width of the image/s" << endl;
		cout << "arg 6 = height of the image/s" << endl;
		cout << "arg 7 = # of copies of the image/s" << endl;
		cout << "arg 8 = flag to decide if consider or not the loading images time in the computation one" << endl;
		cout << "arg 9 = flag to decide if consider or not the saving time in the computation one" << endl;

		return 0;
	}

	ofstream file;
	file.open( "results_seq.txt", ios::app );

	if(!file.is_open()) {
		return 0;
	}

	int dimW        = atoi( argv[5] );
	int dimH        = atoi( argv[6] );
	int imageCopies = atoi( argv[7] );

	char* imagesDir = argv[1];
	char* saveDir   = argv[2];
	char* stampDir  = argv[3];
	char* stampName = argv[4];

	bool stream; stringstream( argv[8] ) >> boolalpha >> stream;
	bool save;  stringstream( argv[9] ) >> boolalpha >> save;

	cout << "argv[0] = " << argv[0] << endl;
	cout << "argv[1] = " << imagesDir << endl;
	cout << "argv[2] = " << saveDir << endl;
	cout << "argv[3] = " << stampDir << endl;
	cout << "argv[4] = " << stampName << endl;
	cout << "argv[5] = " << dimW << endl;
	cout << "argv[6] = " << dimH << endl;
	cout << "argv[7] = " << imageCopies << endl;
	cout << "argv[8] = " << stream << endl;
	cout << "argv[9] = " << save << endl;

	CImg<imageType>* stamp = NULL;

	Timer time;

	vector<Message*> messages;

	for(auto & p : fs::directory_iterator( stampDir )) {
		if(!is_directory( p.path() )) {
			string imageName = (string) stampDir + "/" + p.path().filename().string();
			if(strcmp( p.path().filename().c_str(), stampName ) == 0) {
				stamp = new CImg<imageType>( imageName.c_str() );
				stamp->resize( dimW, dimH );
			}
		}
	}

	if(stream) {
		time.startTime();
		for(int i = 0; i < imageCopies; i++) {
			for(auto & p : fs::directory_iterator( imagesDir )) {
				if(!is_directory( p.path() )) {
					string imageName = p.path().filename().string();
					CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );
					image->resize( dimW, dimH );

					Message* message = new Message( image, imageName );
					messages.push_back( message );

					insertWatermark( stamp, message->getImage() );

					if(save) {
						saveFile( saveDir, message );
						delete message;
					}

					break;
				}
			}
		}
	} else {
		Message* message = NULL;
		for(auto & p : fs::directory_iterator( imagesDir )) {
			if(!is_directory( p.path() ) && imageCopies >= 0) {
				string imageName = p.path().filename().string();
				CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );
				image->resize( dimW, dimH );

				message = new Message( image, imageName );
				break;

				//Message* mex = new Message( image, imageName );
				//messages->push_back( mex );
			}
		}

		for(int k = 0; k < imageCopies; k++) {
			messages.push_back( message->clone() );
		}

		time.startTime();

		for(unsigned int i = 0; i < messages.size(); i++) {
			Message* message = messages.at( i );

			insertWatermark( stamp, message->getImage() );

			if(save) {
				saveFile( saveDir, messages.at( i ) );
				delete message;
			}
		}
	}

	time.stopTime();

	if(!save) {
		for(unsigned int i = 0; i < messages.size(); i++) {
			saveFile( saveDir, messages.at( i ) );
			delete messages.at( i );
		}
	}

	delete stamp;

	string streaming = stream ? " streaming parallel" : "out streaming parallel";
	string saving = save ? " saving images" : "out saving images";
	cout << endl << "Sequential time = " << time.getDuration() << " secs using " << "with" << streaming << " and with" << saving << endl;

	file << time.getDuration() << " " << stream << " " << save << endl;
	file.close();

	return 0;
}
