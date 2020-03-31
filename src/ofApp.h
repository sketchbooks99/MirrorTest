#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxPostProcessing.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void renderScene(bool isMirror = false);
    
    ofShader mirrorShader, objShader;
    ofVboMesh mirror, sphere;
    ofEasyCam cam;
    ofFbo mirrorFbo;
	ofVec3f lightDir;
    ofMatrix4x4 model, view, projection, mvpMatrix, vpMatrix, invMatrix, biasMatrix;
    
    float time;
		
    ofxPanel gui;
    ofParameter<float> reflectOffset;
    ofParameter<float> noiseScale;
    ofParameter<float> noiseStrength;
};
