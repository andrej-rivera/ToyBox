#pragma once
#pragma once

#include  "ofxAssimpModelLoader.h"
#include  "ParticleEmitter.h"
#include "Octree.h"

class Lander {

public:
	// ---lander stuff--- \\
	
	void setup() {
		//setup model
		model.loadModel("geo/lander.obj");
		model.setScaleNormalization(false);
		model.setScale(.5, .5, .5);
		model.setPosition(0, 0, 0);

		landerAngle = model.getRotationAngle(0);
		forces.push_back(&landerAcceleration);
	}

	void integrate();
	void angularIntegrate();
	void drawDebugArrow();

	ofVec3f landerPosition = ofVec3f(0, 50, 0);
	ofVec3f landerVelocity = ofVec3f(0, 0, 0);
	ofVec3f landerAcceleration = ofVec3f(0, 0, 0);

	vector<ofVec3f *> forces;

	ofVec3f headingVector = ofVec3f(0, 0, 1);
	ofVec3f sideVector = ofVec3f(1, 0, 0);

	float landerAngle = 0;
	float angularVelocity = 0;
	float angularAcceleration = 0;

	float landerThrust = 10;
	float damping = 0.98;
	float angularDamping = 0.98;

	bool thrust;

	// ---particle stuff---
	ParticleEmitter emitter;
	ofxAssimpModelLoader model;

	// ---hitbox stuff---
	Box landerBounds;
	vector<Box> collisions;

};
