#pragma once

#include "ofMain.h"
#include "ofxLibwebsockets.h"
#include <ofxXmlSettings.h>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		//Image handling
		void rescaleImage();
		string chooseImage();
		ofImage image;
		int w, h;
		float desiredScreenWidth, percentageX;
		bool showDesiredMargin;
		string image_type;
		int totalImageTypes;
		int imageChoice;
		int imageChange;
		int cyclesPerImage;

		//Timer
		uint64_t startTime;
        uint64_t timeInterval;

        int object_counter;

        //XML data
        vector<string> types;
        vector<int> totalIndexes;
        vector<vector<string>> captions;
        int pipeTotal;
        ofxXmlSettings pipeXML;

        //App config XML
        ofXml configXML;

        //Websocket 
        ofxLibwebsockets::Server server;
        int port;
        string toSend; //data to send to machine via websockets
        void onConnect( ofxLibwebsockets::Event& args );
        void onOpen( ofxLibwebsockets::Event& args );
        void onClose( ofxLibwebsockets::Event& args );
        void onIdle( ofxLibwebsockets::Event& args );
        void onMessage( ofxLibwebsockets::Event& args );
        void onBroadcast( ofxLibwebsockets::Event& args );

        //Protocol Handling
        bool isTextConnected;
        string clientIp;
};
