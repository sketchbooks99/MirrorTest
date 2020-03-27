#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    objShader.load("shader/obj");
    mirrorShader.load("shader/mirror");

    sphere = ofSpherePrimitive(100, 32).getMesh();
    mirror = ofPlanePrimitive(1000, 1000, 100, 100).getMesh();
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
    gui.add(reflectOffset.set("REFLECT OFFSET", 1.0, 0.01, 300.0));
}

//--------------------------------------------------------------
void ofApp::update(){
    time = ofGetElapsedTimef();
    lightDir = ofVec3f(-0.577, 0.577, -0.577);
    
    ofVec3f camPosition = ofVec3f(sin(time * 0.2) * 500.0, 100, cos(time * 0.2) * 500.0);
//    cam.setGlobalPosition(camPosition);
//    cam.lookAt(ofVec3f(0, 0, 0));
    
    cam.begin();
    model = ofMatrix4x4();
    projection = cam.getProjectionMatrix();
    view = ofGetCurrentViewMatrix();
    vpMatrix = view * projection;
    cam.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    glEnable(GL_DEPTH_TEST);
    
    // render scene for mirror texture
    mirrorFbo.begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ofBackground(ofFloatColor(0.3, 0.9, 0.9, 1.0));
    model.translate(0, 100 + abs(sin(time)) * 100, 0);
    mvpMatrix = model * view * projection;
    invMatrix = mvpMatrix.getInverse();
    objShader.begin();
    objShader.setUniform1i("mirror", true);
    objShader.setUniformMatrix4f("mMatrix", model);
    objShader.setUniformMatrix4f("vpMatrix", vpMatrix);
    objShader.setUniformMatrix4f("invMatrix", invMatrix);
    objShader.setUniform3f("camPos", cam.getGlobalPosition());
    objShader.setUniform3f("lightDir", lightDir);
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
    sphere.draw();
    objShader.end();
    
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( cam.getModelViewMatrix() );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * view * projection * biasMatrix;
    
    // render mirror
    mirrorShader.begin();
    model = ofMatrix4x4();
    model.rotate(90, -1, 0, 0);
    mvpMatrix = model * view * projection;
    invMatrix = mvpMatrix.getInverse();
    mirrorShader.setUniformMatrix4f("mvpMatrix", mvpMatrix);
    mirrorShader.setUniformMatrix4f("mvMatrix", model * view);
    mirrorShader.setUniformMatrix4f("transMatrix", shadowTransMatrix);
    mirrorShader.setUniformTexture("tex", mirrorFbo.getTexture(), 0);
    mirrorShader.setUniform1f("time", time);
    mirrorShader.setUniform1f("alpha", 0.5);
    mirrorShader.setUniform1f("offset", reflectOffset);
    mirrorShader.setUniformMatrix4f("invMatrix", invMatrix);
    mirrorShader.setUniform3f("camPos", cam.getGlobalPosition());
    mirrorShader.setUniform3f("lightDir", lightDir);
    mirror.draw();
    mirrorShader.end();
    glDisable(GL_DEPTH_TEST);
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::renderScene(bool isMirror) {
    ofMatrix4x4 model;
    ofMatrix4x4 projection = cam.getProjectionMatrix();
    ofMatrix4x4 view = ofGetCurrentViewMatrix();
    ofMatrix4x4 vpMatrix = view * projection;
    ofMatrix4x4 mvpMatrix = model * view * projection;
    ofMatrix4x4 invMatrix = mvpMatrix.getInverse();

    objShader.begin();
    objShader.setUniform1i("mirror", isMirror);
    objShader.setUniformMatrix4f("mMatrix", model);
    objShader.setUniformMatrix4f("vpMatrix", vpMatrix);
    objShader.setUniformMatrix4f("invMatrix", invMatrix);
    objShader.setUniform3f("camPos", cam.getGlobalPosition());
    objShader.setUniform3f("lightDir", lightDir);
    sphere.draw();
    objShader.end();
}
