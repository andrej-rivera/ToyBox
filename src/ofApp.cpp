#include "ofApp.h"

void ofApp::restart(){
	// setup stats
	startGame = false;
	playerWon = false;
	outtaFuel = false;
	fuelCount = 120.0f;
	altitude = 0;
	score = 0;
	bool platformsLanded[3] = { false, false, false };

	//setup freecam
	freeCamera.setDistance(10);
	freeCamera.setNearClip(.1);
	freeCamera.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	freeCamera.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// camera stuff
	trackingCamera.setPosition({ 0, 12, 16 });
	onboardCamera.setNearClip(0.1f);
	activeCamera = &freeCamera;


	//reset forces
	gravityForce = ofVec3f(0, -5, 0);
	groundForce = ofVec3f(0, 0, 0);
	forwardForce = ofVec3f(0, 0, 0);
	sideForce = ofVec3f(0, 0, 0);

	//setup lander
	lander.setup();
	lander.landerPosition.set(0, 2, 0);
	lander.forces.push_back(&gravityForce);
	lander.forces.push_back(&groundForce);
	lander.forces.push_back(&forwardForce);
	lander.forces.push_back(&sideForce);
	lander.model.setPosition(0, 2, 0);
}

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);



	//setup freecam
	freeCamera.setDistance(10);
	freeCamera.setNearClip(.1);
	freeCamera.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	freeCamera.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// camera stuff
	trackingCamera.setPosition({0, 12, 16});
	onboardCamera.setNearClip(0.1f);
	activeCamera = &freeCamera;

	// lighting
	initLightingAndMaterials();

	// setup background
	background.load("images/stars.jpg");
	background.setAnchorPercent(0.5f, 0.5f);

	// setup map
	map.loadModel("geo/environment.dae");
	map.setScaleNormalization(false);

	// setup octree
	for (int i = 0; i < map.getMeshCount(); i++)
	{
		Octree o;
		o.create(map.getMesh(i), 10);
		octrees.push_back(o);
	}

	//temp gui stuff
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	//gui.add(platformPos.setup("Box Position: ", ofVec3f(2,0,0), ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10)));

	//setup lander
	lander.setup();
	lander.landerPosition.set(0, 2, 0);
	lander.forces.push_back(&gravityForce);
	lander.forces.push_back(&groundForce);
	lander.forces.push_back(&forwardForce);
	lander.forces.push_back(&sideForce);
	lander.model.setPosition(0,2,0);

	//win platform boxes
	platform1 = Box(Vector3(-1.8, 0, 1), Vector3(-0.8, 1, 2));
	platform2 = Box(Vector3(7, 4, -3.5), Vector3(8, 5, -2.5));
	platform3 = Box(Vector3(-8.5, 5.7, -4.8), Vector3(-7.5, 6.7, -3.8));


	platforms.push_back(&platform1);
	platforms.push_back(&platform2);
	platforms.push_back(&platform3);


	// audio
	if (!AudioSystem::isLoaded()) {
		AudioSystem::setup();
	}
	AudioSystem::play(Sound::soundtrack);

	// explosion
	explosionEmitter.amount = 64;
	explosionEmitter.isExplosive = true;
}

//--------------------------------------------------------------
void ofApp::update(){
	if (!startGame)
	{
		lander.landerPosition.set(lander.model.getPosition());
		return;
	}

	playerMove();
	//ofVec3f pos = platformPos;
	//platform2.setOrigin(Vector3(pos.x, pos.y, pos.z));

	//integrate lander
	lander.integrate();
	lander.angularIntegrate();

	//altitude counter
	Ray ray = Ray(Vector3(lander.landerPosition.x, lander.landerPosition.y, lander.landerPosition.z),
		Vector3(0,-1,0));
	float closestDistance = 1000;
	for (Octree& o : octrees)
	{
		TreeNode node;
		bool hit = o.intersect(ray, o.root, node);
		if (!hit)
			break;

		float d = lander.landerBounds.min().length() - node.box.max().length();
		if (d < closestDistance && d > 0)
		{
			closestDistance = d;
		}
	}
	if (closestDistance != 1000)
		altitude = closestDistance;


	//handle collisions between lander & map
	for (Octree& o : octrees)
	{
		o.intersect(lander.landerBounds, o.root, lander.collisions);
	}

	if (lander.collisions.size() > 0)
	{
		lander.collisions.clear();

		lander.landerVelocity.y = -lander.landerVelocity.y;
		lander.landerVelocity.y *= 0.5;

		if (lander.landerVelocity.length() > 4) {
			AudioSystem::play(Sound::explosion);
			lander.landerVelocity.y = lander.landerVelocity.y * 5;
			lander.landerVelocity.x = ofRandom(-10, 10) * 5;
			lander.landerVelocity.z = ofRandom(-10, 10) * 5;
			//cout << lander.landerVelocity << endl;
			explosionEmitter.position = lander.landerPosition;
			explosionEmitter.started = true;
			playerWon = true;
		}
		groundForce = -lander.landerAcceleration + -gravityForce;
	}
	else {
		groundForce = ofVec3f(0, 0, 0);
	}


	//handle landing on win platforms
	for (int i = 0; i < platforms.size(); i++)
	{
		Box b = *platforms[i];
		if (b.overlap(lander.landerBounds) && !platformsLanded[i])
		{
			platformsLanded[i] = true;
			if (lander.landerVelocity.length() > 4){
				score += 500;
			}
			else{
				score += 1000;
			}

		}
	}

	if (platformsLanded[0] && platformsLanded[1] && platformsLanded[2])
	{
		playerWon = true;
		cout << "You Won!" << endl;
	}
	// camera stuff
	if (activeCamera == &trackingCamera) {
		trackingCamera.lookAt(lander.landerPosition);
	}
	else if (activeCamera == &onboardCamera) {
		onboardCamera.setPosition(lander.landerPosition + lander.headingVector * 0.14f);
		onboardCamera.lookAt(lander.landerPosition + lander.headingVector);
		onboardCamera.tilt(-75.0f);
		onboardCamera.dolly(0.3f);
	}

	// emitters
	lander.emitter.position = lander.landerPosition;
	lander.emitter.update();
	explosionEmitter.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofDisableDepthTest();
	ofSetColor(255);
	background.draw(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
	ofEnableDepthTest();

	ofFill();
	ofSetColor(ofColor::blue);

	activeCamera->begin();
	ofPushMatrix();

	ofEnableLighting();              // shaded mode

	keyLight.enable();
	backLight.enable();
	for (size_t i = 0; i < landingPadLights.size(); i++) {
		landingPadLights[i].enable();
	}

	map.drawFaces();
	// map.drawWireframe();

	lander.model.drawFaces();
	lander.drawDebugArrow(); // draws heading vector & side vector arrows

	lander.emitter.draw();
	explosionEmitter.draw();

	// draw octree
	ofNoFill();
	ofSetColor(ofColor::white);
	// for (Octree& o : octrees)
	// {
	// 	o.draw(numLevels, 0);
	// 	o.drawLeafNodes(o.root);

	// }

	ofSetColor(ofColor::green);
	for (Box* b : platforms)
	{
		Octree::drawBox(*b);
	}
	Octree::drawBox(lander.landerBounds);

	ofSetColor(ofColor::red);
	for (Box& collision : lander.collisions)
	{
		Octree::drawBox(collision);
	}

	// disable lighting
	for (size_t i = 0; i < landingPadLights.size(); i++) {
		landingPadLights[i].disable();
	}
	keyLight.disable();
	backLight.disable();
	ofDisableLighting();

	ofPopMatrix();

	activeCamera->end();

	ofDisableDepthTest();

	ofSetColor(ofColor::white);
	
	if (!startGame) {
		ofDrawBitmapStringHighlight("Press Spacebar to Launch! Drag the rocket to move it.", ofGetWindowWidth() / 2 - 150, ofGetWindowHeight() / 2, ofColor::black, ofColor::white);
		ofDrawBitmapStringHighlight("Drag the rocket to move it.", ofGetWindowWidth() / 2 - 50, ofGetWindowHeight() / 2 - 25, ofColor::black, ofColor::white);
	}

	if (playerWon) {
		ofDrawBitmapStringHighlight("Push O to restart.", ofGetWindowWidth() / 2 - 50, ofGetWindowHeight() / 2 - 25, ofColor::black, ofColor::white);
		ofDrawBitmapStringHighlight("Score: " + ofToString(score, 2), ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 - 25, ofColor::black);
	}

	ofDrawBitmapString("Score: " + ofToString(score, 2), 50, 100);
	ofDrawBitmapString("Altitude (AGL): " + ofToString(altitude,2), 50, 125);
	ofDrawBitmapString("Remaining Fuel: " + ofToString(fuelCount,2), 50, 150);

	if (lander.landerVelocity.length() > 4)
		ofSetColor(ofColor::red);

	ofDrawBitmapString("Speed: " + ofToString(lander.landerVelocity.length(), 2), 50, 175);

	gui.draw();
	ofEnableDepthTest();
}

void ofApp::playerMove() {
	if (outtaFuel || !startGame || playerWon)
		return;

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
	
	// Start/Restart Game
	case ' ':
		if (!startGame)
			startGame = true;
		break;

	case 'o':
		if(startGame)
			restart();
		break;

	// Camera controls

	case OF_KEY_TAB:
		if (activeCamera == &freeCamera) {
			activeCamera = &trackingCamera;
		}
		else if (activeCamera == &trackingCamera) {
			activeCamera = &onboardCamera;
		}
		else {
			activeCamera = &freeCamera;
		}
		break;
	case OF_KEY_ALT:
		freeCamera.enableMouseInput();
		break;
	case 'r':
		freeCamera.lookAt(lander.landerPosition);
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

	if (!startGame)
		return;

	// start thruster loop if it's not playing and player is moving
	bool isMoving{false};
	for (size_t i = 0; i < 7; i++) {
		if (keymap[i]) {
			isMoving = true;
			break;
		}
	}
	if (!AudioSystem::isPlaying(Sound::thruster) && isMoving && !outtaFuel) {
		AudioSystem::play(Sound::thruster);
	}

	//fuel guage
	if (isMoving) 
		fuelCount -= ofGetLastFrameTime();
	
	if (fuelCount <= 0)
		outtaFuel = true;

	if (isMoving && !lander.emitter.started) {
		lander.emitter.started = true;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	//lander.landerAcceleration = ofVec3f(0, 0, 0);
	//lander.angularAcceleration = 0;
	switch (key) {
	case OF_KEY_ALT:
		freeCamera.disableMouseInput();
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
		lander.emitter.started = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (freeCamera.getMouseInputEnabled() || startGame) return;

	if (inDrag) {

		glm::vec3 landerPos = lander.model.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, activeCamera->getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;

		landerPos += delta;
		lander.model.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		//ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
		//ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();

		//Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		//colBoxList.clear();
		//octree.intersect(bounds, octree.root, colBoxList);


		/*if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}*/


	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
		if (startGame) return;

		glm::vec3 origin = activeCamera->getPosition();
		glm::vec3 mouseWorld = activeCamera->screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
		ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			selectedLander = true;
			mouseDownPos = getMousePointOnPlane(lander.model.getPosition(), activeCamera->getZAxis());
			mouseLastPos = mouseDownPos;
			inDrag = true;
		}
		else {
			selectedLander = false;
		}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	inDrag = false;
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


	keyLight.setAreaLight(32, 32);
	keyLight.setDiffuseColor(ofColor::blueViolet);
	keyLight.setPosition(32, 32, 0);
	keyLight.lookAt({0, 0, 0});

	backLight.setAreaLight(32, 32);
	backLight.setDiffuseColor(ofColor::lightGray);
	backLight.setPosition(-32, 32, 0);
	backLight.lookAt({0, 0, 0});

	for (size_t i = 0; i < landingPadLights.size(); i++) {
		landingPadLights[i].setPointLight();
		landingPadLights[i].setDiffuseColor(ofColor::red);
		landingPadLights[i].setAttenuation(1.0f, 0.0f, 0.1f);
	}
	landingPadLights[0].setPosition(-1.3f, 1.0f, 1.5f);
	landingPadLights[1].setPosition(7.5f, 4.5f, -3.0f);
	landingPadLights[2].setPosition(-8.0f, 6.2f, -4.3f);
}

glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = activeCamera->getPosition();
	glm::vec3 camAxis = activeCamera->getZAxis();
	glm::vec3 mouseWorld = activeCamera->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
