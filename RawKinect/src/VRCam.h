#pragma once

#include "Kinect.h"
#include "Kinect.Face.h"
#include "ofCamera.h"

class VRCam
{
	public:
		VRCam(IKinectSensor* kinect);
		
		void begin();
		void update();
		void end();
		void zeroOutCamera();

		float scale = 500;

		~VRCam();
	private:
		IKinectSensor* kinect;

		ofCamera camera;

		IBodyFrameReader* bodyFrameReader;
		IBody* bodies;

		IHighDefinitionFaceFrameReader* faceFrameReader;

		IHighDefinitionFaceFrameSource* faceFrameSources[BODY_COUNT];
		IHighDefinitionFaceFrameReader* faceFrameReaders[BODY_COUNT];

		CameraSpacePoint* InitialNose = nullptr;
		CameraSpacePoint diff;

};

