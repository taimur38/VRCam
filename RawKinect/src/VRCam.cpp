#include "VRCam.h"



VRCam::VRCam(IKinectSensor* kinect)
{
	this->kinect = kinect;

	HRESULT hr = E_FAIL;

	BOOLEAN kinect_isOpen = false;
	hr = kinect->get_IsOpen(&kinect_isOpen);
	if (SUCCEEDED(hr) && !kinect_isOpen)
	{
		kinect->Open();
	}

	IBodyFrameSource* bodyFrameSource = nullptr;
	
	hr = kinect->get_BodyFrameSource(&bodyFrameSource);
	if (SUCCEEDED(hr))
	{
		hr = bodyFrameSource->OpenReader(&bodyFrameReader);
	}

	if (SUCCEEDED(hr))
	{
		for (int i = 0; i < BODY_COUNT; i++)
		{
			if (SUCCEEDED(hr))
			{
				hr = CreateHighDefinitionFaceFrameSource(kinect, &faceFrameSources[i]);
			}
			if (SUCCEEDED(hr))
			{
				hr = faceFrameSources[i]->OpenReader(&faceFrameReaders[i]);
			}
		}
	}

	camera.setPosition(ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, -100));
	camera.setupPerspective();
}

void VRCam::update()
{

	HRESULT hr = E_FAIL;
	IBody* ppBodies[BODY_COUNT] = { 0 };

	if (bodyFrameReader != nullptr)
	{
		IBodyFrame* bframe = nullptr;
		hr = bodyFrameReader->AcquireLatestFrame(&bframe);
		if (SUCCEEDED(hr))
		{
			hr = bframe->GetAndRefreshBodyData(BODY_COUNT, ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			for (int iFace = 0; iFace < BODY_COUNT; ++iFace)
			{
				IHighDefinitionFaceFrame* faceFrame = nullptr;
				hr = faceFrameReaders[iFace]->AcquireLatestFrame(&faceFrame);

				BOOLEAN faceTracked = false;
				if (SUCCEEDED(hr) && faceFrame != nullptr)
				{
					faceFrame->get_IsTrackingIdValid(&faceTracked);
				}

				if (faceTracked)
				{
					IFaceAlignment* faceAlignment = nullptr;
					CreateFaceAlignment(&faceAlignment);
					faceFrame->GetAndRefreshFaceAlignmentResult(faceAlignment);

					FaceAlignmentQuality* quality = new FaceAlignmentQuality();
					faceAlignment->get_Quality(quality);
					if (quality == nullptr || *quality != FaceAlignmentQuality_High)
						return;

					float* deformations = new float[FaceShapeDeformations_Count];
					IFaceModel* faceModel = nullptr;
					CreateFaceModel(1.0, FaceShapeDeformations_Count, deformations, &faceModel);
					faceModel->GetFaceShapeDeformations(FaceShapeDeformations_Count, deformations);

					UINT hdvertices = 1347;
					CameraSpacePoint* vertices = new CameraSpacePoint[hdvertices];
					faceModel->CalculateVerticesForAlignment(faceAlignment, hdvertices, vertices);
					//CameraSpacePoint* nose = &vertices[HighDetailFacePoints_NoseTip];
					CameraSpacePoint* nose = new CameraSpacePoint();
					faceAlignment->get_HeadPivotPoint(nose);

					if (InitialNose == nullptr) {
						InitialNose = new CameraSpacePoint();
						InitialNose->X = nose->X;
						InitialNose->Y = nose->Y;
						InitialNose->Z = nose->Z;
						ofLogVerbose("SETTING INITIAL NOSE");
					}


					//CameraSpacePoint* diff = new CameraSpacePoint();
					diff.X = (nose->X - InitialNose->X) * scale;
					diff.Y = (nose->Y - InitialNose->Y) * scale;
					diff.Z = (nose->Z - InitialNose->Z) * scale;

					auto curr = camera.getPosition();
					camera.setPosition(ofVec3f(ofGetWindowWidth() / 2 + diff.X, ofGetWindowHeight() / 2 - diff.Y, 665 + diff.Z));
					//camera.setLensOffset(ofVec2f(-diff.X / 500, diff.Y / 500));
					//camera.lookAt(ofVec3f(-diff.X, diff.Y, diff.Z));
					curr = camera.getPosition();

					//ofLog(OF_LOG_VERBOSE, "initial location is <%.3f, %.3f, %.3f>", InitialNose->X, InitialNose->Y, InitialNose->Z);
					ofLog(OF_LOG_VERBOSE, "nose location is <%.3f, %.3f, %.3f>", diff.X, diff.Y, diff.Z);
					ofLog(OF_LOG_VERBOSE, "cam location is <%.3f, %.3f, %.3f>", curr.x, curr.y, curr.z);
					ofLog(OF_LOG_VERBOSE, "cam diff location is <%.3f, %.3f, %.3f>", curr.x - ofGetWindowWidth() / 2, curr.y - ofGetWindowHeight() / 2, curr.z);

					delete[] vertices;
					delete[] deformations;
					faceModel->Release();
					//HighDetailFacePoints_NoseTip
				}
				else
				{
					IBody* body = ppBodies[iFace];
					if (body != nullptr)
					{
						BOOLEAN bTracked = false;
						hr = body->get_IsTracked(&bTracked);

						UINT64 bodyId;
						if (SUCCEEDED(hr) && bTracked)
						{
							hr = body->get_TrackingId(&bodyId);
							if (SUCCEEDED(hr))
							{
								faceFrameSources[iFace]->put_TrackingId(bodyId);
							}
						}
					}
				}
				
				if (faceFrame != nullptr) {
					faceFrame->Release();
				}
			}
		}

		for (int i = 0; i < _countof(ppBodies); i++) {
			if(ppBodies[i] != nullptr)
				ppBodies[i]->Release();
		}
		if (bframe != nullptr)
			bframe->Release();
	}
}

void VRCam::zeroOutCamera()
{
	InitialNose = nullptr;
}

void VRCam::begin()
{
	camera.begin();
}

void VRCam::end()
{

	camera.end();

}

VRCam::~VRCam()
{
}
