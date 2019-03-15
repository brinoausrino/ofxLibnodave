#include "ofxLibnodave.h"

ofxLibnodave::ofxLibnodave(){
	daveSetDebug(daveDebugPrintErrors);

	areaTypes.insert(make_pair<string, int>("peripheral", daveP));   		/* direct peripheral access */
	areaTypes.insert(make_pair<string, int>("input", daveInputs));
	areaTypes.insert(make_pair<string, int>("outout", daveOutputs));
	areaTypes.insert(make_pair<string, int>("flag", daveFlags));
	areaTypes.insert(make_pair<string, int>("db", daveDB)); 	/* data blocks */
	areaTypes.insert(make_pair<string, int>("di", daveDI)); 	/* instance data blocks */
	areaTypes.insert(make_pair<string, int>("local", daveLocal)); 	/* not tested */
	areaTypes.insert(make_pair<string, int>("v", daveV)); 	/* don't know what it is */
	areaTypes.insert(make_pair<string, int>("counter", daveCounter)); 	/* S7 counters */
	areaTypes.insert(make_pair<string, int>("timer", daveTimer)); 	/* S7 timers */
	areaTypes.insert(make_pair<string, int>("counter200", daveCounter200)); 	/* IEC counters (200 family) */
	areaTypes.insert(make_pair<string, int>("timer200", daveTimer200)); 		/* IEC timers (200 family) */
	areaTypes.insert(make_pair<string, int>("dataS5", daveSysDataS5)); 	/* system data area ? */
	areaTypes.insert(make_pair<string, int>("rawS5", daveRawMemoryS5)); 	/* just the raw memory */
}

ofxLibnodave::~ofxLibnodave()
{
	disconnectPLC();
	disconnectAdapter();
}

/// \brief creates a connection to a SPS of type daveProtoISOTCP with 187k speed
void ofxLibnodave::setup(string ipAdress, int rack, int slot)
{
	fds.rfd = openSocket(102, ipAdress.c_str());
	fds.wfd = fds.rfd;

	int res;
	if (fds.rfd>0) {
		di = daveNewInterface(fds, "IF1", 0, daveProtoISOTCP, daveSpeed187k);
		daveInitAdapter(di);
		daveSetTimeout(di, 5000000);
		dc = daveNewConnection(di, 2, rack, slot);  // insert your rack and slot here

		if (0 == daveConnectPLC(dc)) {
			ofLogNotice("ofxLibnodave::setup", "SPS succesfully connected");

			res = daveReadBytes(dc, daveFlags, 0, 0, 16, NULL);
			if (0 != res) {
				ofLogError("ofxLibnodave::setup", "Couldn't connect SPS!");
			} 	
		} else {
			ofLogError("ofxLibnodave::setup", "Couldn't connect to Programmable Logic Controller");
		}
	} else {
		ofLogError("ofxLibnodave::setup","Couldn't open TCP port. Please make sure a CP (Processor) is connected and the IP address is ok.");
	}
}

/// \brief reades a or multiple bytes from SPS and returns them as ofBuffer
/// \param areaType location to read (input, output, db ...)
/// \param DB data area
/// \param start read from position
/// \param len number of bytes to read
ofBuffer ofxLibnodave::readBytes(string areaType, int DB, int start, int len)
{
	//ofLogNotice(ofGetTimestampString() + " _ " + ofToString(ofGetElapsedTimeMillis()) + " ofxLibnodave::readbytes", "create char");
	char* buf = new char(len);
	//fill with test values
	for (int i = 0; i < len; ++i) buf[i] = 0;

	//ofLogNotice(ofGetTimestampString() + " _ " + ofToString(ofGetElapsedTimeMillis()) + " ofxLibnodave::readbytes", "find areatype");
	if (areaTypes.find(areaType) != areaTypes.end()){
		//ofLogNotice(ofGetTimestampString() + " _ " + ofToString(ofGetElapsedTimeMillis()) + " ofxLibnodave::readbytes", "areatype found, read bytes");
		daveReadBytes(dc, areaTypes[areaType], DB, start, len, buf);
		//ofLogNotice(ofGetTimestampString() + " _ " + ofToString(ofGetElapsedTimeMillis()) + " ofxLibnodave::readbytes", "bytes read");
	}else {
		ofLogError("ofxLibnodave::readBytes","areaType " + areaType + " not a valid area type");
	}
	//ofLogNotice(ofGetTimestampString() + " _ " + ofToString(ofGetElapsedTimeMillis()) + " ofxLibnodave::readbytes", "create buffer");
	ofBuffer b = ofBuffer(buf, len);

	string ret = "return size";
	ret += ofToString(b.size()) + "  ";
	for (size_t i = 0; i < b.size(); i++)	{
		ret += " ";
		auto c = b.getData()[i];
		auto in = int(c);
		ret += in;
	}

	ofLogNotice(ofGetTimestampString() + " _ " + ofToString(ofGetElapsedTimeMillis()) + " ofxLibnodave::readbytes", ret);
	return b;
}

/// \brief reades a byte from SPS and returns it as ofBuffer
ofBuffer ofxLibnodave::readByte(string areaType, int DB, int start)
{
	return readBytes(areaType, DB, start, 1);
}

/// \brief writes one ore multiple bytes to SPS
/// \param areaType location to write (input, output, db ...)
/// \param DB data area
/// \param start write from position
/// \param len number of bytes to write
int ofxLibnodave::writeBytes(string areaType, int DB, int start, int len, ofBuffer buf)
{
	if (areaTypes.find(areaType) != areaTypes.end()){
		int suc = daveWriteBytes(dc, areaTypes[areaType], DB, start, len, buf.getData());
		//cout << suc << endl;
		return suc;
	} else {
		ofLogError("ofxLibnodave::writeBytes","areaType " + areaType + " not a valid area type");
		return -1;
	}
}

/// \brief writes one ore multiple bytes to SPS formated as \param item
int ofxLibnodave::writeBytes(NodaveRequest item)
{
	return writeBytes(item.areaType, item.DB, item.start, item.len, item.buf);
}
/// \brief writes a byte to SPS
int ofxLibnodave::writeByte(string areaType, int DB, int pos, char input)
{
	char* v = &input;
	ofBuffer b = ofBuffer(v, 1);
	return writeBytes(areaType, DB, pos, 1, b);
}

/*ofBuffer ofxLibnodave::readMultipleItems(vector<NodaveRequest> items)
{
	if (items.size() > 20)
	{
		ofLogError("ofxLibnodave::readMultipleItems","maximum 20 items per request");
		return ofBuffer();
	}
	
	PDU p;
	daveResultSet rs;
	davePrepareReadRequest(dc, &p);

	for (auto& item : items) {
		if (areaTypes.find(item.areaType) != areaTypes.end()) {
			daveAddVarToReadRequest(&p, areaTypes[item.areaType], item.DB, item.start, item.len);
		} else {
			ofLogError("ofxLibnodave::readMultipleItems", "areaType " + item.areaType + " not a valid area type");
			return ofBuffer();
		}
	}

	int res = daveExecReadRequest(dc, &p, &rs);

	ofBuffer out;
	for (int  i = 0; i < items.size(); i++)
	{
		res = daveUseResult(dc, &rs, i);
		//here we need to define the type of output for each item
		//daveget
		//out.append()
	}

	return ofBuffer();
}*/

/*int ofxLibnodave::writeMultipleItems(vector<NodaveRequest> items)
{
	int ret = 0;
	for (auto& item:items)
	{
		int r2 = writeBytes(item);
		if (r2 != 0) ret = r2;
	}
	return ret;
}*/


/// \brief ends data transfer
void ofxLibnodave::disconnectPLC()
{
	if (dc!= nullptr)
		daveDisconnectPLC(dc);
}

/// \brief disconnect network adaptor
void ofxLibnodave::disconnectAdapter()
{
	if (di != nullptr)
		daveDisconnectAdapter(di);
}
