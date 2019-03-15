/*
* ofxLibnodave.h
*/

#ifndef OFXLIBNODAVE_H_
#define OFXLIBNODAVE_H_

#include "ofMain.h"
#include "nodave.h"		
#include "openSocket.h"	

typedef union { unsigned short int s; char c[2]; } NodaveWord;


/// \brief a Request for libnodave, only needed for multiple requests
class NodaveRequest {
public:
	NodaveRequest() {

	}

	NodaveRequest(string areaType_, int DB_, int start_, int len_) {
		areaType = areaType_;
		DB = DB_;
		start = start_;
		len = len_;
	}

	NodaveRequest(string areaType_, int DB_, int start_, ofBuffer buf_) {
		areaType = areaType_;
		DB = DB_;
		start = start_;
		len = buf_.size();
		buf = ofBuffer(buf_);
	}

	NodaveRequest(string areaType_, int DB_, int start_, char value) {
		areaType = areaType_;
		DB = DB_;
		start = start_;
		len = 1;
		tC = value;
		buf = ofBuffer(&tC, 1);
	}

	NodaveRequest(string areaType_, int DB_, int start_, NodaveWord word_) {
		areaType = areaType_;
		DB = DB_;
		start = start_;
		len = 2;
		buf = ofBuffer(word_.c,2);
	}

	NodaveRequest(const NodaveRequest &request) {
		areaType = request.areaType;
		DB = request.DB;
		start = request.start;
		len = request.len;
		buf = request.buf;
	}

	~NodaveRequest() {
	}

	string areaType;
	int DB;
	int start;
	int len;
	ofBuffer buf;

	friend ostream& operator<<(ostream& os, const NodaveRequest& dt);

private:
	char tC;
};

inline
ostream& operator<<(ostream& os, const NodaveRequest& p)
{
	os << "areaType : " << p.areaType << "  DB : " << p.DB << "  start : " << p.start << "  len : " << p.len << "  values : ";
	for (size_t i = 0; i < p.len; i++)
	{
		os << " " << int(p.buf.getData()[i]);
	}
	return os;
}

/// \brief provides an interface for libnodave
class ofxLibnodave {
public:
	ofxLibnodave();
	~ofxLibnodave();
	void setup(string ipAdress, int rack = 0, int slot = 2);

	ofBuffer readBytes(string areaType, int DB, int start, int len);
	ofBuffer readByte(string areaType, int DB, int start);
	int writeBytes(string areaType, int DB, int start, int len, ofBuffer buf);
	int writeBytes(NodaveRequest item);
	int writeByte(string areaType, int DB, int pos, char input);

	//ofBuffer readMultipleItems(vector<NodaveRequest> items);
	//int writeMultipleItems(vector<NodaveRequest> items);

	void disconnectPLC();
	void disconnectAdapter();

private:
	daveInterface * di;
	daveConnection * dc;
	_daveOSserialType fds;

	map<string, int> areaTypes;
};

#endif /* OFXLIBNODAVE_H_ */