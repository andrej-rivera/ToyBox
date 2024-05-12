#pragma once
#pragma once

#include  "ofxAssimpModelLoader.h"
#include  "ParticleEmitter.h"

class Lander {

public:
	// ---lander stuff--- \\

	void integrate();
	void angularIntegrate();
	void drawDebugArrow();

	ofVec3f landerPosition = ofVec3f(0, 0, 0);
	ofVec3f landerVelocity = ofVec3f(0, 0, 0);
	ofVec3f landerAcceleration = ofVec3f(0, 0, 0);

	ofVec3f headingVector = ofVec3f(0, 0, 1);
	ofVec3f sideVector = ofVec3f(1, 0, 0);

	float landerAngle = 0;
	float angularVelocity = 0;
	float angularAcceleration = 0;

	float landerThrust = 1;
	float damping = 0.95;
	float angularDamping = 0.9;



	// ---particle stuff---
	ParticleEmitter emitter;
	ofxAssimpModelLoader model;

	bool bBackgroundLoaded = false;
	bool bLanderLoaded = false;
};
