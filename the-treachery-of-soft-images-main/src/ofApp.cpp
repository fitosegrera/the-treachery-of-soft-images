#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(255);

	totalImageTypes = 7;

	//loacal array with types, used to populate the types vector
	string typesArr[totalImageTypes] = {
		"pipe", 
		"broom", 
		"jar", 
		"sponge", 
		"telephone", 
		"hammer", 
		"egg"
	};

	//Config file
	if( configXML.load("config.xml") ){
    	cout<< "Configuration Loaded!"<< endl;
    	timeInterval = configXML.getValue<uint64_t>("timeInterval");
    	percentageX = configXML.getValue<float>("percentageX");
    	showDesiredMargin = configXML.getValue<bool>("showDesiredMargin");
    	port = configXML.getValue<int>("port");
    	clientIp = configXML.getValue<string>("clientIp");
    	cyclesPerImage = configXML.getValue<int>("cyclesPerImage");
	}
	
	//Populate the types vector
	for (int i = 0; i < totalImageTypes; i++){
		types.push_back(typesArr[i]);
	}

	//Load xml files with data from all image_types
	//captions are loaded here in a 2 dimensional vector<vector<string>> captions
	// total image nuber for each image type is stored in the vector vector<int> totalIndexes
	ofxXmlSettings XML[totalImageTypes];

	for(int j=0; j<totalImageTypes; j++){ //for each image_type
		if( XML[j].loadFile(types[j] + "/viz.xml") ){
    	XML[j].pushTag("root"); //iterate through the <root> tag of each viz.xml
	    	totalIndexes.push_back(XML[j].getNumTags("row"));
			cout<< "viz.xml loaded from " + types[j]<< endl;
			cout<< "totalTags: "<< totalIndexes[j]<< endl;
			vector<string> caps; //generate a local vector to stor the caps of this image_type
			for (int i = 0; i < totalIndexes[j]; i++){ //for each element in this image_type
				XML[j].pushTag("row", i); //iterate through all the <row> tags from each viz.xml
					string caption = XML[j].getValue("caption", "");
					caps.push_back(caption);
		        XML[j].popTag();
			}
			captions.push_back(caps); //push the new caps vector to the main 2 dimensional vector
		XML[j].popTag();
		}
	}    

	//WEBSOCKETS CONFIG
	ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = port;
	options.bUseSSL = false; // you'll have to manually accept this self-signed cert if 'true'!
    server.setup( options );
    server.addListener(this);// this adds your app as a listener for the server

	//Start with a random image
	imageChoice = 0;
	imageChange = 0;
	
	image_type = "pipe";//start with the pipe type
	//totalIndexes[0] refers to the total amount of images of the type pipe
	int imageIndex = ofRandom(0, totalIndexes[0]); 

	//send caption of new image to the other screen with the form:
    //imageType&;the rest of the message goes here
    //The text app will split it into type (left of the &;) and content (right of the &;)
	server.send(image_type + "&;" + captions[0][imageIndex]);// 
	image.load(image_type + "/img" + ofToString(imageIndex+1) + ".jpg");
	
	/*
	We need to calculate the pecentage fo the screen that needs to be reduced on X (width).
	The screen is 93cm wide and we want to show a centered 70cm.

	NewWidth = P% * width
	NewWidth = (93-70)% * 93
	NewWidth = 23/100 * 93 = 21.39

	This percentage value is included in the config.xml, if needed 
	modify directly from there.
	*/

	desiredScreenWidth = ofGetScreenWidth() - ((percentageX/100) * ofGetScreenWidth());
	
	rescaleImage();

	//timer setup
	startTime = ofGetElapsedTimeMillis();

	//protocol handling
	isTextConnected = false;
}

//--------------------------------------------------------------
void ofApp::rescaleImage(){

	/*
	http://stackoverflow.com/questions/6565703/math-algorithm-fit-image-to-screen-retain-aspect-ratio

	Image data: (wi, hi) and define ri = wi / hi
	Screen resolution: (ws, hs) and define rs = ws / hs
	rs > ri ? (wi * hs/hi, hs) : (ws, hi * ws/wi) 
	
	Example:

		     20
	|------------------|
	    10
	|---------|

	--------------------     ---   ---
	|         |        |      | 7   |
	|         |        |      |     | 10
	|----------        |     ---    |
	|                  |            |
	--------------------           ---

	ws = 20
	hs = 10
	wi = 10
	hi = 7
	*/ 

	if(h >= w){
		cout<< "HEIGHT-BIGGER"<< endl;
		// w = desiredScreenWidth;
		// h = image.getHeight() * desiredScreenWidth / image.getWidth();
		// h = ofGetScreenHeight() - ofGetScreenHeight() / 3;
		// w = image.getWidth() * ofGetScreenHeight() / h;
		h = ofGetScreenHeight() - ofGetScreenHeight() / 3;
		w = h * image.getWidth() / image.getHeight();		
	}else{
		cout<< "WIDTH-BIGGER"<< endl;
		// float ratio = image.getWidth() / image.getHeight();
		//cout<< "RATIO: " + ofToString(ratio)<< endl;
		// w = image.getWidth() * ofGetScreenHeight() / image.getHeight();
		// while(w < desiredScreenWidth){
		// 	w++;
		// 	// h += 1/ratio;
		// }
		// h = ofGetScreenHeight();
		w = desiredScreenWidth;
		h = w * image.getHeight() / image.getWidth();
	}

	image.resize(w,h);
}

//--------------------------------------------------------------
string ofApp::chooseImage(){
	string type = "";
	// imageChoice = int(ofRandom(0, totalImageTypes));
	imageChoice++;
	if(imageChoice > (totalImageTypes-1)){
		imageChoice = 0;
	}
	cout<< "image_type: "<< types[imageChoice]<< endl;
	return types[imageChoice];
}

//--------------------------------------------------------------
void ofApp::update(){

	if(isTextConnected){
		//Timer interval to load new images
		uint64_t now = ofGetElapsedTimeMillis();
	    if (now - startTime > timeInterval) {
	    	//image_type = chooseImage();
	    	int imageIndex = ofRandom(0, totalIndexes[imageChoice]); //Get random image Number
			image.load(image_type + "/img" + ofToString(imageIndex+1) + ".jpg"); //load new image
	        rescaleImage(); //Rescale the image size to fit the screen
	    	server.send(image_type + "&;" + captions[imageChoice][imageIndex]);// send caption of new image to the other screen
	    	startTime = ofGetElapsedTimeMillis(); //reset timer
	    	cout<< "New Image: " + ofToString(imageIndex)<< endl;
	    	imageChange++; //count how many times the image_type is shown
	    	
	    	//if the image_type cycle is done, change the image type
	    	if(imageChange>cyclesPerImage){
	    		imageChange = 0; 
	    		image_type = chooseImage();
	    	}
	    }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	if(isTextConnected){
		//Draw the image
		ofSetColor(255);
		ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
			image.draw(ofGetScreenWidth()/2 - image.getWidth()/2,
				ofGetScreenHeight()/2 - image.getHeight()/2);
		ofDisableBlendMode();
		//Show Desired margin at sides in green
		if(showDesiredMargin){
			ofSetColor(0,255,0,100);
			ofRect(0,0,((percentageX/100) * ofGetScreenWidth())/2,ofGetScreenHeight());
			ofRect((ofGetScreenWidth()-((percentageX/100) * ofGetScreenWidth())/2),
				0,((percentageX/100) * ofGetScreenWidth())/2,ofGetScreenHeight());
		}
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
    cout<<args.conn.getClientIP()<< endl;

    ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    cout<< args.conn.getClientIP()<< endl;
    
    string ip = args.conn.getClientIP();

    if(ip == clientIp){
    	isTextConnected = false;
    }
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    cout<<"got message: "<<args.message<<endl;

    if(args.message == "$text;"){
    	isTextConnected = true;
    }
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
