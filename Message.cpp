/*
 * Message.cpp
 *
 *  Created on: 25 giu 2018
 *      Author: marco
 */

#include "Message.h"

namespace iwm {

	Message::Message( CImg<imageType>* image, string name ) {
		this->image = image;
		this->name = name;
	}

	Message::Message( string name ) {
		this->image = nullptr;
		this->name = name;
	}

	Message::~Message() {
		if (this->image != nullptr) {
			delete this->image;
		}
	}

	CImg<imageType>* Message::getImage() {
		return image;
	}

	string Message::getName() {
		return name;
	}

} /* namespace iwm */
