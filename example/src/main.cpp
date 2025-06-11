#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	
	
	ofGLFWWindowSettings settings;
	//settings.setGLVersion(3, 2); // Programmable renderer
	
	settings.setSize(1400,800);
	
	settings.setPosition( ofVec2f(0,0) );
	settings.resizable = true;
	
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
	mainWindow->setWindowTitle("SunCalcDemo");
	
	shared_ptr<ofApp> mainApp(new ofApp);
	
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();
	
	
	// OG
	//ofSetupOpenGL(1400,800,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	//ofRunApp(new ofApp());
	

}
