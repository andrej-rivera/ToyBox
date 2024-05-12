#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "ParticleEmitter.h"



class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		
		// ---lander stuff--- \\

		void integrate();
		void angularIntegrate();

		ofVec3f landerPosition = ofVec3f(0,0,0);
		ofVec3f landerVelocity = ofVec3f(0,0,0);
		ofVec3f landerAcceleration = ofVec3f(0,0,0);

		ofVec3f headingVector = ofVec3f(0,0,1);
		ofVec3f sideVector = ofVec3f(1, 0, 0);

		float landerAngle = 0;
		float angularVelocity = 0;
		float angularAcceleration = 0;

		float landerThrust = 1;
		float damping = 0.98;
		float angularDamping = 0.95;



		// ---particle stuff---
		ParticleEmitter emitter;

		ofxFloatSlider rate;
		ofxFloatSlider radius;
		ofxFloatSlider lifespan;
		ofxVec3Slider velocity;
		ofxPanel gui;

		ofEasyCam cam;
		ofxAssimpModelLoader lander;
		ofLight light;
		ofImage backgroundImage;
		ofCamera *theCam = NULL;
		ofCamera topCam;


		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
	
		bool bBackgroundLoaded = false;
		bool bLanderLoaded = false;
};
