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
	map.loadModel("geo/environment.dae");
	map.setScaleNormalization(false);


	//setup octree
	octree.create(map.getMesh(1), 10);


	//temp gui stuff
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));


	//setup lander
	lander.setup();
	lander.forces.push_back(&gravityForce);
	lander.forces.push_back(&groundForce);
	lander.forces.push_back(&forwardForce);
	lander.forces.push_back(&sideForce);

	// audio
	if (!AudioSystem::isLoaded()) {
		AudioSystem::setup();
	}
	AudioSystem::play(Sound::soundtrack);

}

//--------------------------------------------------------------
void ofApp::update(){
	playerMove();

	//integrate lander
	lander.integrate();
	lander.angularIntegrate();

	//handle collisions between lander & map
	octree.intersect(lander.landerBounds, octree.root, lander.collisions);
	if (lander.collisions.size() > 0)
	{
		lander.collisions.clear();
		lander.landerVelocity = -lander.landerVelocity; 
		groundForce = -lander.landerAcceleration + -gravityForce;
	}
	else {
		groundForce = ofVec3f(0, 0, 0);
	}

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
	// map.drawWireframe();

	lander.model.drawFaces();
	lander.drawDebugArrow(); // draws heading vector & side vector arrows

	// draw octree
	ofNoFill();
	ofSetColor(ofColor::white);
	octree.draw(numLevels, 0);
	octree.drawLeafNodes(octree.root);
	Octree::drawBox(lander.landerBounds);

	ofSetColor(ofColor::red);
	for (Box& collision : lander.collisions)
	{
		Octree::drawBox(collision);
	}


	ofPopMatrix();


	cam.end();
}

void ofApp::playerMove() {

	// rotation
	if (keymap[0] == true) { // rotate spacecraft clockwise (about Y (UP) axis)
		lander.angularAcceleration = -lander.landerThrust * 10;
	}
	if (keymap[1] == true) { // rotate spacecraft counter-clockwise (about Y (UP) axis)
		lander.angularAcceleration = lander.landerThrust * 10;
	}

	// Y thrust

	if (keymap[2] == true) { // up movement
		lander.landerAcceleration.y = lander.landerThrust;
	}
	if (keymap[3] == true) { // down movement
		lander.landerAcceleration.y = -lander.landerThrust;
	}

	// XZ thrust
	if (keymap[4] == true) { // forward movement
		forwardForce = lander.headingVector * lander.landerThrust;

	}
	if (keymap[5] == true) { // backward movement
		forwardForce = lander.headingVector * -lander.landerThrust;
	}
	if (keymap[6] == true) { // left movement
		sideForce = -lander.sideVector * lander.landerThrust;
	}
	if (keymap[7] == true) { // right movement
		sideForce = lander.sideVector * lander.landerThrust;
	}



	// stops player movement 
	if (keymap[0] == keymap[1])
	{
		lander.angularAcceleration = 0;
	}
	if (keymap[2] == keymap[3])
	{
		lander.landerAcceleration.y = 0;
	}
	if (keymap[4] == keymap[5])
	{
		forwardForce = ofVec3f(0, 0, 0);
	}
	if (keymap[6] == keymap[7])
	{
		sideForce = ofVec3f(0, 0, 0);
	}

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case OF_KEY_ALT:
		cam.enableMouseInput();
		break;
	
	// ================== lander rotation & Y thrust ================== \\
	
	case 'd': // rotate clockwise
		keymap[0] = true;
		break;
	case 'a': // rotate counterclockwise
		keymap[1] = true;
		break;
	case 'w':     // spacecraft thrust UP
		keymap[2] = true;
		break;
	case 's':     // spacefraft thrust DOWN
		keymap[3] = true;
		break;

	// ================== lander XZ thrust ================== \\
	
	case OF_KEY_UP:    // move forward
		keymap[4] = true;
		break;
	case OF_KEY_DOWN:   // move  backwards
		keymap[5] = true;
		break;
	case OF_KEY_LEFT:   // move left
		keymap[7] = true;
		break;
	case OF_KEY_RIGHT:   // move right
		keymap[6] = true;
		break;

	default:
		break;
	}


	// start thruster loop if it's not playing and player is moving
	bool isMoving{false};
	for (size_t i = 0; i < 7; i++) {
		if (keymap[i]) {
			isMoving = true;
			break;
		}
	}
	if (!AudioSystem::isPlaying(Sound::thruster) && isMoving) {
		AudioSystem::play(Sound::thruster);
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	//lander.landerAcceleration = ofVec3f(0, 0, 0);
	//lander.angularAcceleration = 0;
	switch (key) {
	case OF_KEY_ALT:
		cam.disableMouseInput();
		break;

		// ================== lander rotation & Y thrust ================== \\
		
	case 'd': // rotate clockwise
		keymap[0] = false;
		break;
	case 'a': // rotate counterclockwise
		keymap[1] = false;
		break;
	case 'w':     // spacecraft thrust UP
		keymap[2] = false;
		break;
	case 's':     // spacefraft thrust DOWN
		keymap[3] = false;
		break;

		// ================== lander XZ thrust ================== \\
		
	case OF_KEY_UP:    // move forward
		keymap[4] = false;
		break;
	case OF_KEY_DOWN:   // move  backwards
		keymap[5] = false;
		break;
	case OF_KEY_LEFT:   // move left
		keymap[7] = false;
		break;
	case OF_KEY_RIGHT:   // move right
		keymap[6] = false;
		break;

	default:
		break;
	}

	// stop thruster loop if the player isn't moving
	bool isMoving{false};
	for (size_t i = 0; i < 7; i++) {
		if (keymap[i]) {
			isMoving = true;
			break;
		}
	}
	if (!isMoving) {
		AudioSystem::stop(Sound::thruster);
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