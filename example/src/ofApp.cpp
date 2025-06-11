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
	
	
	if(ofIsGLProgrammableRenderer()) {
		bool success = gradientShader.load("shaders/GL3/gradient");
		ofLogNotice("ProgrammableRenderer") << success;
	}else{
		bool success = gradientShader.load("shaders/GL2/gradient");
		ofLogNotice("Non-ProgrammableRenderer") << success;
	}
	
	if (!gradientShader.isLoaded()) {
		ofLogError() << "Gradient shader failed to load";
	}

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::updateDebugStrings( Poco::DateTime &date ) {
    
	date_str = Poco::DateTimeFormatter::format(ofxSunCalc::offsetDate(date, tz_offset), "%Y-%m-%d %H:%M:%S");
    
	sun_pos = sun_calc.getSunPosition(date, lat, lon);
    
	alt_str = "altitude = " + ofToString(sun_pos.altitude);
	azi_str = "azimuth =  " + ofToString(sun_pos.azimuth * RAD_TO_DEG);
    
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
	
	Poco::DateTime local_now = ofxSunCalc::offsetDate(now, tz_offset);
	// Bulid local time so we set the marker accordingly.
    
	
    // Draw background gradient based on sun_brightness
    ofColor nightBG(ofColor::black);
	ofColor nightFG(64);
    
	ofColor dayBG(ofColor::deepSkyBlue);
	ofColor dayFG(ofColor::paleGoldenRod);
    
    ofColor background = nightBG.lerp(dayBG, sun_brightness);
	ofColor midground = nightFG.lerp(dayFG, sun_brightness);
	ofColor foreground = ofColor::white; // nightFG.lerp(dayFG, sun_brightness);
    
    //ofBackgroundGradient(foreground, background);
	float w = ofGetWidth();
	float h = ofGetHeight();
	float cx = w/2;
	
	float sx = cx; // TODO move x based on sun_pos.azimuth, and the lon being the centre of screen
	sx = cx + (lon + sun_pos.azimuth * RAD_TO_DEG); // TOOD: this not yet working, ask chatgpt for the algorithm
	
	float sy = (h * sun_pos.altitude);
	
	// TODO: manipulate radi based on brightness etc. too
	float innerRadius = 50.0;
	float middleRadius = 300.0;
	float outerRadius = std::max(ofGetWidth(), ofGetHeight()) * 0.5;

	ofVec2f sunCenterPos(sx, sy); // where on screen to put the sun TODO: link this in with our alititude and x,y for period in the day, east to west (Right to Left)
	// circularBackgroundGradient(foreground, background, sunGradientPos);
	
	//ofLog() << "Colors: fg=" << foreground << ", mg=" << midground << ", bg=" << background;
	
	ofSetColor(255);
	
	ofFill();
	
	gradientShader.begin();

	   gradientShader.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
	   gradientShader.setUniform2f("u_center", sunCenterPos.x, sunCenterPos.y );
	   gradientShader.setUniform1f("u_innerRadius", innerRadius);
	   gradientShader.setUniform1f("u_middleRadius", middleRadius);
	   gradientShader.setUniform1f("u_outerRadius", outerRadius);

	   gradientShader.setUniform3f("u_colorInner", foreground.r / 255.0f, foreground.g / 255.0f, foreground.b / 255.0f);
	   gradientShader.setUniform3f("u_colorMiddle", midground.r / 255.0f, midground.g / 255.0f, midground.b / 255.0f);
	   gradientShader.setUniform3f("u_colorOuter", background.r / 255.0f, background.g / 255.0f, background.b / 255.0f);

	   ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	gradientShader.end();
	
    ofDrawBitmapStringHighlight(date_str, 15, 20, ofColor::paleGoldenRod, ofColor::black);
    
    ofDrawBitmapStringHighlight(min_info_str, 15, 45, ofColor::salmon, ofColor::white);
    
    ofDrawBitmapStringHighlight(max_info_str, 15, 125, ofColor::darkOrange, ofColor::white);
    
    ofDrawBitmapStringHighlight(latlon_str, 195, 20, ofColor::gold, ofColor::black);
    
    ofDrawBitmapStringHighlight(alt_str, 195, 45, ofColor::cornsilk, ofColor::black);
	ofDrawBitmapStringHighlight(azi_str, 195, 60, ofColor::cornsilk, ofColor::black);
    
    ofDrawBitmapStringHighlight("Brightness " + ofToString(sun_brightness, 3), 195, 80, ofColor::goldenRod, ofColor::white);
    
    float tx = 10 + 110;
	
	float th = timelines[0].getHeight() + 25;
	
	float ty = ofGetHeight() - (th * timelines.size()); // start pos. so they are bottom aligned
	
	ofDrawBitmapStringHighlight("[Sun Timelines]", 10, ty-10, ofColor::gold, ofColor::black);
	
    for(int i = 0; i<timelines.size(); i++) {
        
        ofSetColor(255);
        timelines[i].draw(tx, ty);
        
        ofDrawBitmapStringHighlight(labels[i], 10, ty+13);
        
        if(i == 2) { // Today!
		
            ofNoFill();
            ofSetLineWidth(1.0);
            ofSetColor(255,0,0);
            ofDrawRectangle(tx, ty, timelines[i].getWidth(), timelines[i].getHeight());
            
            // Draw a current time mark
            float pixels_per_min = (timelines[i].getWidth() / 24) / 60.0;
            float nx = tx + pixels_per_min * (local_now.hour() * 60 + local_now.minute());
            ofSetColor(255, 0, 0);
            ofSetLineWidth(2.0);
            ofDrawLine(nx, ty, nx, ty+timelines[i].getHeight());
        }
        
        ty += th;
    }
    
}

//--------------------------------------------------------------
// Take from ofGraphics::
void ofApp::circularBackgroundGradient(const ofFloatColor & start, const ofFloatColor & end, glm::vec2 center) {
	
	float w = ofGetViewportWidth(), h = ofGetViewportHeight();
	circularGradientMesh.clear();
	circularGradientMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
#ifndef TARGET_EMSCRIPTEN
	#ifdef TARGET_OPENGLES
	if (ofIsGLProgrammableRenderer()) gradientMesh.setUsage(GL_STREAM_DRAW);
	#else
	circularGradientMesh.setUsage(GL_STREAM_DRAW);
	#endif
#endif

	// this could be optimized by building a single mesh once, then copying
	// it and just adding the colors whenever the function is called.
	
	circularGradientMesh.addVertex(glm::vec3(center, 0.f));
	circularGradientMesh.addColor(start);
	float n = 32; // circular gradient resolution
	float angleBisector = glm::two_pi<float>() / (n * 2.0);
	float smallRadius = ofDist(0, 0, w / 2, h / 2);
	float bigRadius = smallRadius / std::cos(angleBisector);
	for (int i = 0; i <= n; i++) {
		float theta = i * glm::two_pi<float>() / n;
		circularGradientMesh.addVertex(glm::vec3(center + glm::vec2(std::sin(theta), std::cos(theta)) * bigRadius, 0));
		circularGradientMesh.addColor(end);
	}

	GLboolean depthMaskEnabled;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMaskEnabled);
	glDepthMask(GL_FALSE);
	circularGradientMesh.draw();
	if (depthMaskEnabled) {
		glDepthMask(GL_TRUE);
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
