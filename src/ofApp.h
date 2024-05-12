#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Octree.h"
#include "Lander.h"
#include  "ofxAssimpModelLoader.h"
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/closest_point.hpp>

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

		ofxAssimpModelLoader map;
		Lander lander;
		ofEasyCam cam;

		// collision objects
		Octree octree;
		Box boundingBox, landerBounds;
		vector<Box> colBoxList;
		vector<Box> bboxList;

		// temp gui objects
		ofxIntSlider numLevels;
		ofxPanel gui;

		void initLightingAndMaterials();
};
