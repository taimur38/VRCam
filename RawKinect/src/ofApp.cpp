#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	/* of boiler stuff*/

	ofSetWindowShape(3840, 2160);
	ofBackground(0);

	light.setDiffuseColor(ofFloatColor(0.85, 0.85, 0.85));
	light.setSpecularColor(ofFloatColor(1.f, 1.f, 1.f));

	ofEnableDepthTest();
	ofSetSphereResolution(48);
	ofSetSmoothLighting(true);
	ofDisableAlphaBlending();
	ofSetLogLevel(OF_LOG_VERBOSE);

	HRESULT hr;
	hr = GetDefaultKinectSensor(&kinect);
	if (FAILED(hr))
		return;

	if (!kinect)
		return;
	
	camera = new VRCam(kinect);

	for (int i = 0; i < num_spheres; ++i)
	{
		ofSpherePrimitive s;
		s.setRadius(50);
		float x = ofRandom(-0, 4000);
		float y = ofRandom(-2000, 2000);
		float z = ofRandom(-1000, 300);
		s.setPosition(x, y, z);
		spheres[i] = s;
	}
}

//--------------------------------------------------------------
void ofApp::update()
{
	camera->update();
}

void ofApp::mousePressed(int x, int y, int button)
{
	camera->zeroOutCamera();
	ofLogVerbose("mouse clicked");
}

//--------------------------------------------------------------
void ofApp::draw()
{
	//ofLog(OF_LOG_VERBOSE, "offset: <%f.2, %f.2>", offset.x, offset.y);
	//ofLog(OF_LOG_VERBOSE, "look dir: <%.2f, %.2f, %f>", lookdir.x, lookdir.y, lookdir.z);
	light.enable();
	camera->begin();
	for (int i = 0; i < num_spheres; ++i)
	{
		spheres[i].draw();
	}
	camera->end();
	light.disable();
}