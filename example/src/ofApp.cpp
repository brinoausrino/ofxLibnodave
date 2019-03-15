#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
	
	// connect with ip address of sps
	sps.setup("192.168.1.10");
	
}

//--------------------------------------------------------------
void ofApp::update(){

	//read a single byte
	ofBuffer single;
	single = sps.readBytes("db", 144, 50, 1);
	ofLogNotice("byte : " + ofToSTring(int(sFree.getData()[0])));

	//read a word
	ofBuffer word = sps.readBytes("db", 200, 16, 2);

	NodaveWord w;
	w.c[0] = word.getData()[1];
	w.c[1] = word.getData()[0];
	ofLogNotice("word : " + ofToSTring(int(w.s)));

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
	ofDrawBitmapString("SPS Software is Running", 10, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
