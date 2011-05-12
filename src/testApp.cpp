
#include "testApp.h"



#define		kSampleRate				44100
#define		kBufferSize				4096
#define		kBitsPerChar			8
#define		kSampleRepeatCount		128	// must be evenly divisible into kBufferSize / kBitsPerChar
#define		kCharsPerBuffer			(kBufferSize / (kSampleRepeatCount * kBitsPerChar))
#define		kAmp					1

float		audioBuffer[kBufferSize*2];	// interleaved audio buffer (for drawing)

ofBuffer	data;
int			charIndex				= 0;
string		parsedMsg;
bool        doSimple                = false;

//--------------------------------------------------------------
void testApp::setup() {	
    ofSetFrameRate(60);
	ofBackground(30,30,30);	
	ofSetVerticalSync(true);
	
	ofSoundStreamSetup(2, 0, this, kSampleRate, kBufferSize, 2);
}


//--------------------------------------------------------------
void testApp::update() {
}


//--------------------------------------------------------------
void testApp::draw() {
	float lineWidth = ofGetWidth() * 1.0f/ kBufferSize;
	
    //	float backCol1	= 0.2 * 255;
	float backCol2	= 0.3 * 255;
	
	float scaler = 0.8;
	ofNoFill();
	
    
	// draw waveforms
	for(int j=0; j<2; j++) {
		ofPushMatrix();
		ofTranslate(0, ofGetHeight()/4 * (1 + 2*j), 0);
        //		ofSetColor(backCol1, backCol1, backCol1);
		ofSetColor(255, 255, 255);
		ofLine(0, 0, ofGetWidth(), 0);
		ofScale(1, scaler, 1);
		for (int i = 0; i < kBufferSize; i++){
			ofSetColor(backCol2, backCol2, backCol2);
			if(i%kBitsPerChar==0) ofLine(i*lineWidth, -ofGetHeight()/4, i*lineWidth, ofGetHeight()/4);
            ofSetColor(255, 255, 255);
			ofRect(i*lineWidth, 0, lineWidth, audioBuffer[i*2+j]*ofGetHeight()/4);
		}
		ofPopMatrix();
	}
    
    // draw button
    if(ofGetMousePressed(0)) {
        ofFill();
        ofSetColor(128, 0, 0);
        ofRect(5, 5, 130, 30);
    }
    ofNoFill();
    ofSetColor(128, 128, 128);
    ofRect(5, 5, 130, 30);
    ofDrawBitmapString("LOAD data.txt", 15, 22);
	
	// draw text with wrap
	int x=140, y=0;
	for(int i=0; i<parsedMsg.size(); i++) {
		ofDrawBitmapString(string("") + parsedMsg[i], x+10, y+15);
		x += 9;
		if(x > ofGetWidth()-50) {
			x=140;
			y+=14;
		}
	}
}


//--------------------------------------------------------------
void testApp::audioRequested(float *output, int bufferSize, int nChannels){
	int index = 0;				// index in output array
	static int clock = 1;		// toggles between -1...1
	
	for(int j=0; j<kCharsPerBuffer; j++) {
		
		// if we have characters left to do
		if(charIndex < data.size()) {
			char curChar = data.getBinaryBuffer()[charIndex];
			parsedMsg += curChar;
			printf("%c (%i) [", curChar, curChar);
			
			for(int bit=0; bit<kBitsPerChar; bit++) {
				int curBit = curChar%2;
				printf("%i", curBit);
				
				float ldata = kAmp * ofMap(curBit, 0, 1, -1, 1);		// left channel is the data (least significant bit first - dumped to console in reverse order!)
				float rdata = kAmp * clock;								// right channel is the clock
				
                //                ldata = 1;
                //                rdata = -1;
                
                if(doSimple) {
                    if((int)floor(ofGetElapsedTimef()) % 2 == 0) {
                        ldata = rdata = -1;
                    } else {
                        ldata = rdata = 1;
                    }
                }
                
				for(int rep=0; rep<kSampleRepeatCount; rep++) {
					output[index*2]			= ldata;
					audioBuffer[index*2]	= ldata;
					
					output[index*2+1]		= rdata;
					audioBuffer[index*2+1]	= rdata;
					
					index++;
				}
				
				curChar>>=1;
				clock *= -1;
			}
			charIndex++;
			printf("]\n");
			
			
			// we have no characters left, just send nil
		} else {
            mousePressed(0, 0, 0);
            
			for(int bit=0; bit<kBitsPerChar; bit++) {
				for(int rep = 0; rep<kSampleRepeatCount; rep++) {
					output[index*2]			=
					audioBuffer[index*2]	= 
					
					output[index*2+1]		= 
					audioBuffer[index*2+1]	= 0;
					index++;
				}
			}
			
		}
	}
	//	printf("\n");
}

void testApp::mousePressed( int x, int y, int button ) {
	charIndex = 0;
	parsedMsg = "";
	data = ofBufferFromFile("data.txt", false);
	printf("\n****************************\n%i bytes read\n", (int)data.size());
}

void testApp::keyPressed(int key) {
    doSimple ^= true;
}

