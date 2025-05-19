#include "ofApp.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timespan.h"

/*
    ofxSunCalc Example App
 
    TODO:  
        * Show the azimuth postition (angle of sun)
        * Sun rise / set position in sky (against horizon?)
        * Draw and extended timeline
		* Show Moon position visually too.
 
 */

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("ofxSunCalc Example");
    
    ofSetFrameRate(30);
    
    
    // Sydney, Australia
    lat = -33.8647; // Note southern degrees need to be - (not like those from google maps)
    lon = 151.2117; //
	
    latlon_str = "lat:" + ofToString(lat) + ", lon:" + ofToString(lon);
    
    
	Poco::LocalDateTime local_now;
	tz_offset = local_now.tzd(); // Format dates to machines local date. !Note: make this 0 if you want to keep dates UTC fixed.
							     // TODO: could be in timezone of the coordinates??
	ofLogNotice("tz_offset") << tz_offset;
	
	Poco::DateTime now;
    updateDebugStrings( now );
    
    ofLogNotice("NOW") << date_str;
    //ofLogNotice("local tzd") << now.tzd();
    
    ofLogNotice("today") << sun_calc.dateToString(now, tz_offset);
    ofLogNotice("sunpos") << pos_str;
    ofLogNotice() << min_info_str << endl << endl << max_info_str;
    
    small_font.load(OF_TTF_MONO, 8, false);
    
    Poco::DateTime sixMonthsAgo = now - Poco::Timespan(30*6, 0, 0, 0, 0);
    Poco::DateTime threeMonthsAgo = now - Poco::Timespan(30*3, 0, 0, 0, 0);
    Poco::DateTime threeMonthsInFuture = now + Poco::Timespan(30*3, 0, 0, 0, 0);
    
    labels.push_back("6 months ago\n" +  ofxSunCalc::dateToDateString(sixMonthsAgo, tz_offset));
    labels.push_back("3 months ago\n" +  ofxSunCalc::dateToDateString(threeMonthsAgo, tz_offset));
    labels.push_back("Today\n" +  ofxSunCalc::dateToDateString(now, tz_offset));
    labels.push_back("3 months time\n" +  ofxSunCalc::dateToDateString(threeMonthsInFuture, tz_offset));
    
    vector<SunCalcDayInfo> sun_infos;
    
    sun_infos.push_back(sun_calc.getDayInfo(sixMonthsAgo, lat, lon, false, tz_offset));
    sun_infos.push_back(sun_calc.getDayInfo(threeMonthsAgo, lat, lon, false, tz_offset));
    sun_infos.push_back(todayInfo);
    sun_infos.push_back(sun_calc.getDayInfo(threeMonthsInFuture, lat, lon, false, tz_offset));
    
    // create/draw a timeline for each date
    for(int i = 0; i<4; i++) {
        timelines.push_back(ofFbo());
        timelines[i].allocate(ofGetWidth() - 20 - 110, 32);
        ofxSunCalc::drawSimpleDayInfoTimeline(timelines[i], sun_infos[i]);
    }

}

//--------------------------------------------------------------
void ofApp::update(){

}

void ofApp::updateDebugStrings( Poco::DateTime &date ) {
    
	date_str = Poco::DateTimeFormatter::format(ofxSunCalc::offsetDate(date, tz_offset), "%Y-%m-%d %H:%M:%S");
    
    SunCalcPosition sunpos = sun_calc.getSunPosition(date, lat, lon);
    
    pos_str = "altitude=" + ofToString(sunpos.altitude) + ", azimuth=" + ofToString(sunpos.azimuth * RAD_TO_DEG);
    
    todayInfo = sun_calc.getDayInfo(date, lat, lon, true, tz_offset);
    
    min_info_str = sun_calc.infoToString(todayInfo, true);
    max_info_str = sun_calc.infoToString(todayInfo, false);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    Poco::DateTime now; //(2015,5,29,17,38); // LocalDateTime
	
    
    if(ofGetKeyPressed(OF_KEY_ALT)) {
        // auto step the time of day to proof changes
       // int total_min = fabs(sin(ofGetElapsedTimef()*.05)) * 1440;
        int total_min = (int)((ofGetElapsedTimef()*.05) * 1440) % 1440;// 1440 == mins per day  60 * 24
        int hr = floor(total_min/60.0);
        int mn = total_min - (hr*60); //now.minute();
        now.assign(now.year(), now.month(), now.day(), hr, mn);
    }
	
	
    updateDebugStrings( now );
    
	float sun_brightness = ofxSunCalc::getSunBrightness(todayInfo, now, tz_offset);
    
    if(ofGetKeyPressed(OF_KEY_COMMAND)) {
        sun_brightness = fabs(sin(ofGetElapsedTimef()*.1));
    }
	
	Poco::DateTime local_now = ofxSunCalc::offsetDate(now, tz_offset); // bulid local time so we set the marker accordingly.
    
    // draw background gradient based on sun_brightness
    
    ofColor nightBG(ofColor::black);
    ofColor nightFG(64);
    
    ofColor dayBG(ofColor::skyBlue);
    ofColor dayFG(ofColor::paleGoldenRod);
    
    ofColor background = nightBG.lerp(dayBG, sun_brightness);
    ofColor foreground = nightFG.lerp(dayFG, sun_brightness);
    
    ofBackgroundGradient(foreground, background);
    
    ofDrawBitmapStringHighlight(date_str, 15, 20, ofColor::paleGoldenRod, ofColor::black);
    
    ofDrawBitmapStringHighlight(min_info_str, 15, 45, ofColor::salmon, ofColor::white);
    
    ofDrawBitmapStringHighlight(max_info_str, 15, 125, ofColor::darkOrange, ofColor::white);
    
    ofDrawBitmapStringHighlight(latlon_str, 195, 20, ofColor::gold, ofColor::black);
    
    ofDrawBitmapStringHighlight(pos_str, 195, 45, ofColor::cornsilk, ofColor::black);
    
    ofDrawBitmapStringHighlight("Current Brightness " + ofToString(sun_brightness, 3), 195, 70, ofColor::goldenRod, ofColor::white);
    
    float tx = 10 + 110;
    float ty = 320;
    for(int i = 0; i<timelines.size(); i++) {
        
        ofSetColor(255);
        timelines[i].draw(tx, ty);
        
        ofDrawBitmapStringHighlight(labels[i], 10, ty+13);
        
        if(i == 2) { // Today!
		
            ofNoFill();
            ofSetLineWidth(1.0);
            ofSetColor(255);
            ofDrawRectangle(tx, ty, timelines[i].getWidth(), timelines[i].getHeight());
            
            // Draw a current time mark
            float pixels_per_min = (timelines[i].getWidth() / 24) / 60.0;
            float nx = tx + pixels_per_min * (local_now.hour() * 60 + local_now.minute());
            ofSetColor(255, 0, 0);
            ofSetLineWidth(2.0);
            ofDrawLine(nx, ty, nx, ty+timelines[i].getHeight());
        }
        
        ty += timelines[i].getHeight() + 25;
    }
    
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
