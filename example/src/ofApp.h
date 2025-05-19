#pragma once

#include "ofMain.h"
#include "ofxSunCalc.h"

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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        void updateDebugStrings( Poco::DateTime &date );
    
        ofxSunCalc sun_calc;
        SunCalcDayInfo todayInfo;
        
        string min_info_str;
        string max_info_str;
        string pos_str;
        string date_str;
        string latlon_str;
        
        vector<ofFbo> timelines;
        vector<string> labels;
	
		int tz_offset = 0;
        
        float lat;
        float lon;
    
        ofTrueTypeFont small_font;
    
};
