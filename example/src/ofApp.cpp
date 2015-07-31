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
 
 */

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("ofxSunCalc Example");
    
    ofSetFrameRate(30);
    
    Poco::LocalDateTime now;
    
    date_str = Poco::DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S");
    ofLogNotice("NOW") << date_str;
    //ofLogNotice("local tzd") << now.tzd();
    
    // Sydney, Australia
    lat = -33.8647; // Note southern degrees need to be - (not like those from google maps)
    lon = 151.2117; //
    
    latlon_str = "lat:" + ofToString(lat) + ", lon:" + ofToString(lon);
    
    ofLogNotice("today") << sun_calc.dateToString(now);
    
    SunCalcPosition sunpos = sun_calc.getSunPosition(now, lat, lon);
    
    pos_str = "altitude=" + ofToString(sunpos.altitude) + ", azimuth=" + ofToString(sunpos.azimuth * RAD_TO_DEG);
    
    ofLogNotice("sunpos") << pos_str;
    
    todayInfo = sun_calc.getDayInfo(now, lat, lon, true);
    
    min_info_str = sun_calc.infoToString(todayInfo, true);
    max_info_str = sun_calc.infoToString(todayInfo, false);
    ofLogNotice() << min_info_str << endl << endl << max_info_str;
    
    small_font.loadFont(OF_TTF_MONO, 8, false);
    
    Poco::LocalDateTime sixMonthsAgo = now - Poco::Timespan(30*6, 0, 0, 0, 0);
    Poco::LocalDateTime threeMonthsAgo = now - Poco::Timespan(30*3, 0, 0, 0, 0);
    Poco::LocalDateTime threeMonthsInFuture = now + Poco::Timespan(30*3, 0, 0, 0, 0);
    
    labels.push_back("6 months ago\n" +  ofxSunCalc::dateToDateString(sixMonthsAgo));
    labels.push_back("3 months ago\n" +  ofxSunCalc::dateToDateString(threeMonthsAgo));
    labels.push_back("Today\n" +  ofxSunCalc::dateToDateString(now));
    labels.push_back("3 months time\n" +  ofxSunCalc::dateToDateString(threeMonthsInFuture));
    
    
    vector<SunCalcDayInfo> sun_infos;
    
    sun_infos.push_back(sun_calc.getDayInfo(sixMonthsAgo, lat, lon, false));
    sun_infos.push_back(sun_calc.getDayInfo(threeMonthsAgo, lat, lon, false));
    sun_infos.push_back(todayInfo);
    sun_infos.push_back(sun_calc.getDayInfo(threeMonthsInFuture, lat, lon, false));
    
    // create/draw a timeline for each date
    for(int i = 0; i<4; i++) {
        timelines.push_back(ofFbo());
        timelines[i].allocate(ofGetWidth() - 20 - 110, 32);
        ofxSunCalc::drawSimpleDayInfoTimeline(timelines[i], sun_infos[i], small_font);
    }

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    Poco::LocalDateTime now; //(2015,5,29,17,38);
    
    if(ofGetKeyPressed(OF_KEY_ALT)) {
        // auto step the time of day to proof changes
        int total_min = fabs(sin(ofGetElapsedTimef()*.05)) * 1440; // 1440 == mins per day  60 * 24
        int hr = floor(total_min/60.0);
        int mn = total_min - (hr*60); //now.minute();
        now.assign(now.year(), now.month(), now.day(), hr, mn);
    }
    
    float sun_brightness = ofxSunCalc::getSunBrightness(todayInfo, now);
    
    if(ofGetKeyPressed(OF_KEY_COMMAND)) {
        sun_brightness = fabs(sin(ofGetElapsedTimef()*.1));
    }
    
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
    
    ofDrawBitmapStringHighlight(max_info_str, 15, 125, ofColor::darkorange, ofColor::white);
    
    ofDrawBitmapStringHighlight(latlon_str, 180, 20, ofColor::gold, ofColor::black);
    
    ofDrawBitmapStringHighlight(pos_str, 180, 45, ofColor::cornsilk, ofColor::black);
    
    ofDrawBitmapStringHighlight("Current Brightness " + ofToString(sun_brightness, 3), 180, 70, ofColor::goldenRod, ofColor::white);
    
    float tx = 10 + 110;
    float ty = 320;
    for(int i = 0; i<timelines.size(); i++) {
        
        ofSetColor(255);
        timelines[i].draw(tx, ty);
        
        ofDrawBitmapStringHighlight(labels[i], 10, ty+13);
        
        if(i == 2) { // today
            ofNoFill();
            ofSetLineWidth(1.0);
            ofSetColor(255);
            ofRect(tx, ty, timelines[i].getWidth(), timelines[i].getHeight());
            
            // Draw a current time mark
            float pixels_per_min = (timelines[i].getWidth() / 24) / 60.0;
            float nx = tx + pixels_per_min * (now.hour() * 60 + now.minute());
            ofSetColor(255, 0, 0);
            ofSetLineWidth(2.0);
            ofLine(nx, ty, nx, ty+timelines[i].getHeight());
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
