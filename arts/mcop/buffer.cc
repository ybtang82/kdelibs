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

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#include "buffer.h"
#include "string.h"
#include <cassert>

using namespace std;

Buffer::Buffer() {
	rpos = 0;
	_readError = false;
}

bool Buffer::readError() {
	return _readError;
}

void Buffer::writeBool(bool b) {
	contents.push_back(b?1:0);
}

void Buffer::writeLong(long l) {
	contents.push_back(l & 0xff);
	contents.push_back((l >> 8) & 0xff);
	contents.push_back((l >> 16) & 0xff);
	contents.push_back((l >> 24) & 0xff);
}

void Buffer::writeLongSeq(const vector<long>& seq) {
	writeLong(seq.size());

	vector<long>::const_iterator i;
	for(i = seq.begin(); i != seq.end(); i++) writeLong(*i);
}

void Buffer::writeFloat(float f) {
	// FIXME: on some machines this may fail badly (there is explicit
	// float marshalling and demarshalling code in mico/orb/util.cc)

	long *f_as_long = (long *)&f;
	writeLong(*f_as_long);
}

void Buffer::writeString(const string& s) {
	long len = s.size()+1;

	writeLong(len);
	contents.insert(contents.end(),reinterpret_cast<const unsigned char*>(s.c_str()),
		        reinterpret_cast<const unsigned char*>(s.c_str()+len));
}

void Buffer::writeStringSeq(const vector<string>& seq) {
	writeLong(seq.size());

	vector<string>::const_iterator i;
	for(i = seq.begin(); i != seq.end(); i++) writeString(*i);
}

void Buffer::write(void *data, long len) {
	unsigned char *c = (unsigned char *)data;

	contents.insert(contents.end(),c,c+len);
}


void *Buffer::read(long l) {
	void *result = 0;

	if(remaining() >= l) {
		result = &contents[rpos];
		rpos += l;
	} else {
		_readError = true;
	}
	return result;
}

void *Buffer::peek(long l) {
	assert(remaining() >= l);
	return &contents[rpos];
}

void Buffer::skip(long l) {
	if(remaining() >= l) {
		rpos += l;
	} else {
		_readError = true;
	}
}

bool Buffer::readBool()
{
	long result = false;
	if(remaining() >= 1) {
		if(contents[rpos] == 1)
			result = true;
		else
		{
			assert(contents[rpos] == 0);
		}
		rpos += 1;
	} else {
		_readError = true;
	}
	return result;
}

long Buffer::readLong()
{
	long result = 0;
	if(remaining() >= 4) {
		result = contents[rpos]
		       + (contents[rpos+1] << 8)
			   + (contents[rpos+2] << 16)
			   + (contents[rpos+3] << 24);
		rpos += 4;
	} else {
		_readError = true;
	}
	return result;
}

void Buffer::readLongSeq(vector<long>& result)
{
	// might be optimizable a bit
	long i,seqlen = readLong();

	result.clear();
	for(i=0;i<seqlen;i++) result.push_back(readLong());
}

float Buffer::readFloat()
{
	// FIXME: see writeFloat()
	long f_as_long = readLong();

	if(!_readError) return *(float *)&f_as_long;
	return 0.0;
}

void Buffer::readString(string& result)
{
	long len = readLong();
	char *data = (char *)read(len);

	if(data && len) result.assign(data,len-1);
}

void Buffer::readStringSeq(vector<string>& result)
{
	// might be optimizable a bit

	long i,seqlen = readLong();

	result.clear();
	//result.reserve(seqlen);

	for(i=0;i<seqlen;i++) {
		string s;

		readString(s);
		result.push_back(s);
	}
}


void Buffer::patchLength()
{
	long len = size();
	assert(len > 8);

	contents[4] = len & 0xff;
	contents[5] = (len >> 8) & 0xff;
	contents[6] = (len >> 16) & 0xff;
	contents[7] = (len >> 24) & 0xff;
}

string Buffer::toString(const string& name)
{
	string result;
	char hex[17] = "0123456789abcdef";

	vector<unsigned char>::iterator ci;
	for(ci = contents.begin(); ci != contents.end(); ci++)
	{
		result += hex[(*ci >> 4) & 0xf];
		result += hex[*ci & 0xf];
	}

	if(name == "") return result;
	return name + ":" + result;
}

unsigned char Buffer::fromHexNibble(char c)
{
	int uc = (unsigned char)c;

	if(uc >= '0' && uc <= '9') return uc - (unsigned char)'0';
	if(uc >= 'a' && uc <= 'f') return uc + 10 - (unsigned char)'a';
	if(uc >= 'A' && uc <= 'F') return uc + 10 - (unsigned char)'A';

	return 16;	// error
}

static int stringncmp(const string& s1, const string& s2, size_t n)
{
	// I don't know a way to implement that compliant to all STL string
	// implementations (compare seems to work differently sometimes)
	return strncmp(s1.c_str(),s2.c_str(),n);
}

bool Buffer::fromString(const string& data, const string& name)
{
	string start = name+":";
	if(name == "") start = "";

	if(stringncmp(data,start,start.size()) != 0) return false;
	contents.clear();

	string::const_iterator di = data.begin() + start.size();
	
	while(di != data.end())
	{
		unsigned char h = fromHexNibble(*di++);	// high nibble
		if(di == data.end()) return false;

		unsigned char l = fromHexNibble(*di++);	// low nibble

		if(h >= 16 || l >= 16) return false;	// no proper hex digit
		contents.push_back((h << 4) + l);
	}
	return true;
}
