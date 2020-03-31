#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    objShader.load("shader/obj");
    mirrorShader.load("shader/mirror.vert", "shader/mirror.frag", "shader/mirror.geom");

    sphere = ofSpherePrimitive(8, 32).getMesh();
    mirror = ofPlanePrimitive(100, 100, 100, 100).getMesh();
    for(int i = 0; i < mirror.getVertices().size(); i++) {
        ofVec3f v = mirror.getVertex(i);
        float tmp = v.y;
        v.y = v.z;
        v.z = tmp;
        mirror.setVertex(i, v);
        mirror.setNormal(i,ofVec3f(0, 1, 0));
    }
//    mirrorFbo.allocate(ofGetWidth(), ofGetHeight());
    ofFbo::Settings fboSettings;
    fboSettings.width = ofGetWidth();
    fboSettings.height = ofGetHeight();
    fboSettings.textureTarget = GL_TEXTURE_2D;
    fboSettings.internalformat = GL_RGB;
    fboSettings.useDepth = true;
    fboSettings.depthStencilAsTexture = true;
    fboSettings.useStencil = true;
    fboSettings.minFilter = GL_LINEAR;
    fboSettings.maxFilter = GL_LINEAR;
    fboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;
    fboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    mirrorFbo.allocate(fboSettings);

    lightDir = ofVec3f(0, 0, 1);
    
    biasMatrix = ofMatrix4x4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
    );
    
    // GUI settings
    gui.setup();
    gui.setPosition(0, 0);
    gui.add(isNormal.set("NORMAL", false));
    gui.add(noiseScale.set("NOISE SCALE", 0.1, 0.001, 1.0));
    gui.add(noiseStrength.set("NOISE STRENGTH", 10.0, 0.1, 100.0));
}

//--------------------------------------------------------------
void ofApp::update(){
    time = ofGetElapsedTimef();
    lightDir = ofVec3f(-0.577, 0.577, 0.577);
    
    ofVec3f camPosition = ofVec3f(sin(time * 0.2) * 500.0, 100, cos(time * 0.2) * 500.0);
//    cam.setGlobalPosition(camPosition);
//    cam.lookAt(ofVec3f(0, 0, 0));
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    model = ofMatrix4x4();
    projection = cam.getProjectionMatrix();
    view = ofGetCurrentViewMatrix();
    vpMatrix = view * projection;
    cam.end();
    
    glEnable(GL_DEPTH_TEST);
    
    // render scene for mirror texture
    mirrorFbo.begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ofBackground(ofFloatColor(0.3, 0.9, 0.9, 1.0));
    model.translate(0, 10 + abs(sin(time)) * 10, 0);
    mvpMatrix = model * view * projection;
    invMatrix = model.getInverse();
    objShader.begin();
    objShader.setUniform1i("mirror", true);
    objShader.setUniformMatrix4f("mMatrix", model);
    objShader.setUniformMatrix4f("vpMatrix", vpMatrix);
    objShader.setUniformMatrix4f("invMatrix", invMatrix);
    objShader.setUniform3f("camPos", cam.getGlobalPosition());
    objShader.setUniform3f("lightDir", lightDir);
    objShader.setUniform1i("isNormal", isNormal);
    sphere.draw();
    objShader.end();
    mirrorFbo.end();
    
//    ofBackground(ofFloatColor(0.0, 0.7, 0.7, 1.0));
    ofBackground(0);
    
    // render scene
    objShader.begin();
    objShader.setUniform1i("mirror", false);
    objShader.setUniformMatrix4f("mMatrix", model);
    objShader.setUniformMatrix4f("vpMatrix", vpMatrix);
    objShader.setUniformMatrix4f("invMatrix", invMatrix);
    objShader.setUniform3f("camPos", cam.getGlobalPosition());
    objShader.setUniform3f("lightDir", lightDir);
    objShader.setUniform1i("isNormal", isNormal);
    sphere.draw();
    objShader.end();
    
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( cam.getModelViewMatrix() );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * view * projection * biasMatrix;
    
    // render mirror
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    mirrorShader.begin();
    model = ofMatrix4x4();
    mvpMatrix = model * view * projection;
    invMatrix = model.getInverse();
    mirrorShader.setUniformMatrix4f("mvpMatrix", mvpMatrix);
    mirrorShader.setUniformMatrix4f("mvMatrix", model * view);
    mirrorShader.setUniformMatrix4f("transMatrix", shadowTransMatrix);
    mirrorShader.setUniformTexture("tex", mirrorFbo.getTexture(), 0);
    mirrorShader.setUniform1f("noiseScale", noiseScale);
    mirrorShader.setUniform1f("noiseStrength", noiseStrength);
    mirrorShader.setUniform1f("time", time);
    mirrorShader.setUniform1f("alpha", 0.5);
    mirrorShader.setUniformMatrix4f("invMatrix", invMatrix);
    mirrorShader.setUniform3f("camPos", cam.getGlobalPosition());
    mirrorShader.setUniform3f("lightDir", lightDir);
    mirrorShader.setUniform1i("isNormal", isNormal);
    mirror.draw();
    mirrorShader.end();
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::renderScene(bool isMirror) {
    ofMatrix4x4 model;
}
