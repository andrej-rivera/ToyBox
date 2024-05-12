#include "Lander.h"

void Lander::integrate() {


	landerPosition += landerVelocity / ofGetFrameRate();
	landerVelocity += landerAcceleration / ofGetFrameRate();
	landerVelocity *= damping;

	model.setPosition(landerPosition.x, landerPosition.y, landerPosition.z);
	//cout << landerAcceleration << endl;
}

// integrator function for rotating the lander
void Lander::angularIntegrate() {
	// ofGetFrameRate() returns 0 on startup, causing division by 0 error.
	// solution: add 1 to ofGetFrameRate() before division

	landerAngle += angularVelocity / (ofGetFrameRate() + 1);
	angularVelocity += angularAcceleration / (ofGetFrameRate() + 1);
	angularVelocity *= angularDamping;
	model.setRotation(0, landerAngle, 0, 1, 0);

	// calculate heading vector & side vector
	float rad = glm::radians(landerAngle);
	headingVector.set(glm::sin(rad), 0, glm::cos(rad));
	sideVector = headingVector.getRotated(90, ofVec3f(0, 1, 0));

	//cout << headingVector << endl;
}

void Lander::drawDebugArrow() {
	ofSetColor(ofColor::green);
	ofDrawArrow(model.getPosition(), model.getPosition() + (headingVector * 2), 0.5);

	ofSetColor(ofColor::red);
	ofDrawArrow(model.getPosition(), model.getPosition() + (sideVector * 2), 0.5);
}