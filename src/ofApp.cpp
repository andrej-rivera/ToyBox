#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//setup cam
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	initLightingAndMaterials();


	//setup map
	map.loadModel("geo/moon-houdini.obj");
	map.setScaleNormalization(false);

	//setup lander
	lander.model.loadModel("geo/lander.obj");
	lander.model.setScaleNormalization(false);
	lander.model.setScale(.5, .5, .5);
	lander.model.setPosition(0, 100, 0);

	lander.landerAngle = lander.model.getRotationAngle(0);

	//setup octree
	octree.create(map.getMesh(0), 10);


	//temp gui stuff
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));

}

//--------------------------------------------------------------
void ofApp::update(){
	lander.integrate();
	lander.angularIntegrate();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(ofColor::black);

	ofFill();
	ofSetColor(ofColor::blue);
	gui.draw();


	cam.begin();
	ofPushMatrix();
	ofEnableLighting();              // shaded mode
	map.drawFaces();

	lander.model.drawFaces();
	lander.drawDebugArrow(); // draws heading vector & side vector arrows

	// draw octree
	ofNoFill();
	ofSetColor(ofColor::white);
	octree.draw(numLevels, 0);

	ofPopMatrix();


	cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case OF_KEY_ALT:
		cam.enableMouseInput();
		break;
	
	// ================== lander rotation ================== \\
	
	case 'd':     // rotate spacecraft clockwise (about Y (UP) axis)
		lander.angularAcceleration -= lander.landerThrust * 20;
		break;
	case 'a':     // rotate spacecraft counter-clockwise (about Y (UP) axis)
		lander.angularAcceleration += lander.landerThrust * 20;
		break;
	case 'w':     // spacecraft thrust UP
		lander.landerAcceleration.y += lander.landerThrust;
		break;
	case 's':     // spacefraft thrust DOWN
		lander.landerAcceleration.y -= lander.landerThrust;
		break;

	// ================== lander movement ================== \\
	
	case OF_KEY_UP:    // move forward
		lander.landerAcceleration += lander.headingVector * lander.landerThrust;
		//landerAcceleration += headingVector * landerThrust;
		break;
	case OF_KEY_DOWN:   // move backward
		//landerAcceleration += ofVec3f(0, 0, landerThrust);
		lander.landerAcceleration -= lander.headingVector * lander.landerThrust;
		break;
	case OF_KEY_LEFT:   // move left
		lander.landerAcceleration -= lander.sideVector * lander.landerThrust;
		break;
	case OF_KEY_RIGHT:   // move right
		lander.landerAcceleration += lander.sideVector * lander.landerThrust;
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case OF_KEY_ALT:
		cam.disableMouseInput();
		break;
	default:
		break;
	}
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

void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}