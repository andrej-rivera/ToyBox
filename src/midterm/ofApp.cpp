
//--------------------------------------------------------------
//
//  10.15.19
//
//  CS 134 Midterm - Fall 2019 - Startup Files
// 
//   author:  Kevin M. Smith   

#include "ofApp.h"

// integrator function for general physics-based movement
void ofApp::integrate() {

	
	landerPosition += landerVelocity / ofGetFrameRate();
	landerVelocity += landerAcceleration / ofGetFrameRate();
	landerVelocity *= damping;
	
	lander.setPosition(landerPosition.x, landerPosition.y, landerPosition.z);
	//cout << landerAcceleration << endl;
}

// integrator function for rotating the lander
void ofApp::angularIntegrate() {
	// ofGetFrameRate() returns 0 on startup, causing division by 0 error.
	// solution: add 1 to ofGetFrameRate() before division
	
	landerAngle += angularVelocity / (ofGetFrameRate() + 1);
	angularVelocity += angularAcceleration / (ofGetFrameRate() + 1);
	angularVelocity *= angularDamping;
	lander.setRotation(0, landerAngle, 0, 1, 0);
	// calculate heading vector & side vector
	float rad = glm::radians(landerAngle);
	headingVector.set(-glm::sin(rad), 0, glm::cos(rad));
	sideVector = headingVector.getRotated(90, ofVec3f(0,1,0));


	//cout << headingVector << endl;
}

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {


	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;

	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	cam.disableMouseInput();

	topCam.setNearClip(.1);
	topCam.setFov(65.5);
	topCam.setPosition(0, 15, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));

	// set current camera;
	//
	theCam = &cam;

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// load BG image
	//
	bBackgroundLoaded = backgroundImage.load("images/starfield-plain.jpg");


	// setup rudimentary lighting 
	//
	initLightingAndMaterials();


	// load lander model
	//
	if (lander.loadModel("geo/lander.obj")) {
		lander.setScaleNormalization(false);
		lander.setScale(.5, .5, .5);
		//lander.setRotation(0, -180, 1, 0, 0);
		lander.setPosition(0, 0, 0);

		bLanderLoaded = true;
	}
	else {
		cout << "Error: Can't load model" << "geo/lander.obj" << endl;
		ofExit(0);
	}

	// setup LEM & emitter
	//
	landerAngle = lander.getRotationAngle(0);

	
	emitter.position = ofVec3f(0, 0, 0);
	emitter.lastSpawned = ofGetElapsedTimeMillis();

	// setup GUI
	gui.setup();

	gui.add(velocity.setup("Velocity", ofVec3f(0, 10, 0), ofVec3f(0, 0, 0), ofVec3f(100, 100, 100)));
	gui.add(lifespan.setup("Lifespan", 2.5, 0, 5));
	gui.add(rate.setup("Rate", 100, 1, 200));
	gui.add(radius.setup("Radius", .25, .1, 1));

}





void ofApp::update() {

	// integrate lander
	integrate();
	angularIntegrate();

	// update emitter based on GUI
	emitter.position = ofVec3f(landerPosition.x, landerPosition.y - 0.2, landerPosition.z);
	emitter.lifespan = lifespan;
	emitter.velocity = ofVec3f(velocity);
	emitter.rate = rate;
	emitter.radius = radius;

	// update emitter
	emitter.update();

	// activate based on if force is being applied
	if (landerAcceleration != ofVec3f(0, 0, 0))
		emitter.started = true;
	else
		emitter.started = false;

	//cout << landerThrust << endl;
}

//--------------------------------------------------------------
void ofApp::draw() {


	// draw background image
	//
	if (bBackgroundLoaded) {
		ofPushMatrix();
		ofDisableDepthTest();
		ofSetColor(50, 50, 50);
		ofScale(2, 2);
		backgroundImage.draw(-200, -100);
		ofEnableDepthTest();
		ofPopMatrix();
	}

	theCam->begin();

	ofPushMatrix();

		// draw a reference grid
		//
		ofPushMatrix();
		ofRotate(90, 0, 0, 1);
		ofSetLineWidth(1);
		ofSetColor(ofColor::dimGray);
		ofDisableLighting();
		ofDrawGridPlane();
		ofPopMatrix();

	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		if (bLanderLoaded) {

			//  Note this is how to rotate LEM model about the "Y" or "UP" axis
			//
//			lander.setRotation(0, angle, 0, 1, 0);    
			lander.drawWireframe();
		}
	}
	else {
		ofEnableLighting();              // shaded mode
		if (bLanderLoaded) {
//			lander.setRotation(0, angle, 0, 1, 0);
			lander.drawFaces();

		}
	}


	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
	}

	
	ofPopMatrix();

	// draw direction arrows & emitter
	ofSetColor(ofColor::green);
	ofDrawArrow(lander.getPosition(), lander.getPosition() + (headingVector * 2), 0.5);

	ofSetColor(ofColor::red);
	ofDrawArrow(lander.getPosition(), lander.getPosition() + (sideVector * 2), 0.5);

	emitter.draw();

	theCam->end();

	ofDisableDepthTest();
	ofDisableLighting();

	// draw GUI
	ofSetColor(ofColor::blue);
	gui.draw();

	// draw screen data
	//
	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);

	string str2;
	str2 += "Altitide (AGL): " + std::to_string(-lander.getPosition().y);
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str2, 5, 15);
}


// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'P':
	case 'p':
		break;
	case 'r':
		cam.reset();
		break;
	case 'g':
		savePicture();
		break;
	case 't':
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'm':
		toggleWireframeMode();
		break;
	case 'd':     // rotate spacecraft clockwise (about Y (UP) axis)
		angularAcceleration -= landerThrust * 20;
		break;
	case 'a':     // rotate spacecraft counter-clockwise (about Y (UP) axis)
		angularAcceleration += landerThrust * 20;
		break;
	case 'w':     // spacecraft thrust UP
		landerAcceleration.y -= landerThrust;
		break;
	case 's':     // spacefraft thrust DOWN
		landerAcceleration.y += landerThrust;
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:    // move forward
		landerAcceleration += headingVector * landerThrust;
		//landerAcceleration += headingVector * landerThrust;
		break;
	case OF_KEY_DOWN:   // move backward
		//landerAcceleration += ofVec3f(0, 0, landerThrust);
		landerAcceleration -= headingVector * landerThrust;


		break;
	case OF_KEY_LEFT:   // move left
		landerAcceleration -= sideVector * landerThrust;
		break;
	case OF_KEY_RIGHT:   // move right
		landerAcceleration += sideVector * landerThrust;
		break;
	default:

		break;
	}
}


void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}


void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}
	landerAcceleration.set(0, 0, 0);
	angularAcceleration = 0;
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

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
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ .7f, .7f, .7f, 1.0f };

	static float position[] =
	{20.0, 20.0, 20.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
//	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
