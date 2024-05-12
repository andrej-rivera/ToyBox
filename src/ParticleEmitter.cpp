
#include "ParticleEmitter.h"

//============PARTICLE============\\

Particle::Particle() {

	// initialize particle with base values
	velocity.set(0, 0, 0);
	acceleration.set(0, 10, 0);
	position.set(0, 0, 0);
	lifespan = 2.5;
	birthtime = 0;
	radius = .01;
	damping = .99;
	mass = 1;

	color = ofColor::red;
}

void Particle::draw() {

	ofSetColor(color);
	ofDrawSphere(position, radius);
}

void Particle::integrate() {
	position += velocity / ofGetFrameRate();
	velocity += acceleration / ofGetFrameRate();
	velocity *= damping;
}

//============PARTICLE EMITTER============\\

ParticleEmitter::ParticleEmitter() {
	// initialize emitter with base values
	rate = 100;
	radius = 0.25;
	velocity = ofVec3f(0, 10, 0);
	lifespan = 2.5;
	started = false;
	lastSpawned = 0;
	amount = 25;
}

void ParticleEmitter::integrate() {
	for (int i = 0; i < particles.size(); i++)
		particles[i].integrate();
}

void ParticleEmitter::draw() {
	for (int i = 0; i < particles.size(); i++)
		particles[i].draw();
}

// spawns new particles
void ParticleEmitter::update() {
	// kill old particles & integrate alive ones
	kill();
	integrate();

	if (!started) return;


	float time = ofGetElapsedTimeMillis();

	if ((time - lastSpawned) > (1000.0 / rate)) {

		// loop to spawn multiple particles in a batch
		for (int i = 0; i < amount; i++)
		{
			Particle particle;

			// change particle color randomly
			int color = (int) ofRandom(0, 3);

			switch (color)
			{
			case 0:
				particle.color = ofColor::yellow;
				break;
			case 1:
				particle.color = ofColor::orange;
				break;
			default:
				particle.color = ofColor::red;
				break;
			}

			// position particle on emitter radius & assign vars
			ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(0, 1), ofRandom(-1, 1));
			particle.velocity = velocity;
			particle.position.set(position + (dir.getNormalized() * radius));
			particle.birthtime = time;
			particle.lifespan = lifespan;

			// add turbulence to particle for some randomness
			ofVec3f turbulence = ofVec3f(ofRandom(-1, 1), 0, ofRandom(-1, 1));
			particle.acceleration *= turbulence;

			particles.push_back(particle);
		}

		lastSpawned = time;

	}

	
}

// kills old particles
void ParticleEmitter::kill() {
	if (particles.size() == 0) return;

	vector<Particle>::iterator p = particles.begin();
	vector<Particle>::iterator tmp;

	while (p != particles.end()) {
		if (p->lifespan != -1 && p->age() > p->lifespan) {
			tmp = particles.erase(p);
			p = tmp;
		}
		else p++;
	}
}

