    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifndef COMMON_H
#define COMMON_H

#include "buffer.h"
#include "type.h"
#include "dispatcher.h"
#include "object.h"
#include "connection.h"
#include "objectmanager.h"
#include "idlfilereg.h"

/* define this to see what mcop transfers around */
#undef DEBUG_IO

/* define this to see what mcop messages are processed */
#undef DEBUG_MESSAGES

template<class T>
void freeTypeSeq(vector<T *>& sequence) {
	for(unsigned long l=0;l<sequence.size();l++)
		delete sequence[l];

	sequence.clear();
};

template<class T>
void readTypeSeq(Buffer& stream, vector<T *>& sequence) {
	freeTypeSeq(sequence);

	unsigned long l = stream.readLong();
	while(l--) sequence.push_back(new T(stream));
};

template<class T>
void writeTypeSeq(Buffer& stream, const vector<T *>& sequence) {
	stream.writeLong(sequence.size());
	for(unsigned long l=0;l<sequence.size();l++)
		sequence[l]->writeType(stream);
};

#include "core.h"

#endif
