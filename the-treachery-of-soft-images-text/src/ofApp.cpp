#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(255);

	//Config file
    if( XML.load("config.xml") ){
        cout<< "config.xml loaded!"<< endl;
        address = XML.getValue<string>("address");
        port = XML.getValue<int>("port");
        fontSize = XML.getValue<int>("fontSize");
        percentageX = XML.getValue<int>("percentageX");
        showDesiredMargin = XML.getValue<bool>("showDesiredMargin");
    }

    //WEBSOCKETS CONFIG
	ofSetLogLevel(OF_LOG_VERBOSE);
	client.connect(address, port);
	ofSetLogLevel(OF_LOG_ERROR);
	client.addListener(this);

	/*
	We need to calculate the pecentage fo the screen that needs to be reduced on X (width).
	The screen is 93cm wide and we want to show a centered 70cm.

	NewWidth = P% * width
	NewWidth = (93-70)% * 93
	NewWidth = 23/100 * 93 = 21.39

	This percentage value is included in the config.xml, if needed 
	modify directly from there.
	*/

	desiredScreenWidth = (percentageX/100) * ofGetScreenWidth();

	//timer
	lastReconnectTime = ofGetElapsedTimeMillis();

	messageArrived = false;
    drawText = false;

	//font 
	ofTrueTypeFont::setGlobalDpi(72);
    font.load("helvetica.ttf", fontSize, true, true);
	inputMessage = "";
}

//--------------------------------------------------------------
string ofApp::makeTextBox(string sToSplit){
    string lines = "";
    string singleLine = "";

    //The message from the main application server comes in the form:
    //imageType&;the rest of the message goes here
    //So lets split it into type (left of the &;) and s (right of the &;)

    string type = ofSplitString(sToSplit, "&;")[0];
    string s = ofSplitString(sToSplit, "&;")[1];
    string newString = "This is not a " + type + ", it's " + s;
    int arrLength = ofSplitString(newString, " ").size();

    for(int i=0; i<arrLength; i++){
        ofRectangle r = font.getStringBoundingBox(singleLine, 0, 0);
        if(r.width > ofGetWidth()-desiredScreenWidth*3){ //I multiply by 3 times so that we get an extra margin
            singleLine += ofSplitString(newString, " ")[i] + " \n";
            lines += singleLine;
            singleLine = "";
        }else{
            if(i == arrLength-1){
                lines = lines + singleLine + ofSplitString(newString, " ")[i] + ".";
            }else{
                singleLine += ofSplitString(newString, " ")[i] + " ";
            }
        }
    }

    return lines;
}

//--------------------------------------------------------------
void ofApp::update(){
	if(client.isConnected()){
		if(messageArrived){
			drawText = true;
			messageArrived = false;
		}
    }else{
        uint64_t now = ofGetElapsedTimeMillis();
        if (now - lastReconnectTime > 5000) {
            cout<< "Reconnecting..."<< endl;
            client.close();
            client.connect(address, port); 
            lastReconnectTime = ofGetElapsedTimeMillis();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(drawText){
        ofBackground(255);
        ofSetColor(0);
        font.drawStringCentered(makeTextBox(inputMessage), ofGetWidth()/2, ofGetHeight()/2);
    }

    if(showDesiredMargin){
        ofSetColor(0,255,0,100);
        ofRect(0,0,((percentageX/100) * ofGetScreenWidth())/2,ofGetScreenHeight());
        ofRect((ofGetScreenWidth()-((percentageX/100) * ofGetScreenWidth())/2),
            0,((percentageX/100) * ofGetScreenWidth())/2,ofGetScreenHeight());
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    showDesiredMargin = !showDesiredMargin;
}       

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"new connection open"<<endl;
    client.send("$text;");
    ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    cout<<"got message: "<<args.message<<endl;
    
    inputMessage = args.message;
    messageArrived = true;
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
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
