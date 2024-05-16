#pragma once
#include "ofMain.h"

// particle class for emitter
class Particle {
public:
	Particle();
	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration;
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	float   birthtime;

	ofColor color;

	void    integrate();
	void    draw();
	float 	age() {
		return (ofGetElapsedTimeMillis() - birthtime) / 1000.0;
	}
};

// emitter class - purpose is to emit particles in a radius
class ParticleEmitter {
public:

	ParticleEmitter();
	ofVec3f position;
	ofVec3f velocity;
	float radius; // size of emitter disk
	float rate; // particles per second
	int amount; // amount of particles per 'volley'
	float lifespan;

	float lastSpawned;


	bool started;

	vector<Particle> particles; // stores particles

	void update();
	void integrate();
	void draw();
	void kill();

};
