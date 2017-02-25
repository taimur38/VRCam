#pragma once

#include "ofMain.h"
#include "Kinect.h"
#include "Kinect.Face.h"
#include "VRCam.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void mousePressed(int x, int y, int button);

	private:
		IKinectSensor* kinect;

		VRCam* camera;
		int num_spheres = 1000;
		ofSpherePrimitive* spheres = new ofSpherePrimitive[num_spheres];
		ofLight light;

};
