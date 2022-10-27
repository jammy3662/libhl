#pragma once

struct Camera
{
	vec3 anchor; // camera origin
	vec3 position; // camera final position
	vec3 upVector;
	float pitch, yaw, roll;
	
	float fov;
	float nearClip;
	float farClip;
	
	float sensX = 0.1;
	float sensY = 0.1;
	
	// third person orbital camera
	int8 orbit = false;
	float orbitDist = 1; // distance anchor -> orbit point
	vec3 orbitOffset; // distance orbit point -> camera position
	
	void init(vec3 _position, vec3 _rotation, float _fov,
		int8 _orbitDist, int8 _orbitOffset)
	{
		anchor = position;
		upVector = {0,1,0};
		pitch = rotation.y; yaw = rotation.x; roll = rotation.z;
		
		fov = _fov;
		nearClip = 0.01;
		farClip = 1000;
		
		sensX = 0.1; sensY = 0.1;
		
		orbit = false;
		orbitDist = _orbitDist;
		orbitOffset = _orbitOffset;
	}
	
	void refresh()
	{
		Quaternion rotation = QuaternionFromEuler(pitch, yaw, roll);

		// position + looking direction
		camera.target = Vector3Add(
		anchor,
		Vector3RotateByQuaternion(
			{0,0,1}, rotation));

		if (orbit)
		{
		// position - looking direction * orbit distance
		camera.position = Vector3Subtract(
			anchor,
			Vector3RotateByQuaternion(
				{0,0,orbitDist * orbit}, rotation));
		camera.position = Vector3Add(camera.position, orbitOffset);
		}
		else camera.position = anchor;
	}
};


/*
struct Camera:  public virtual Node
{	
	// raylib member fields
	Camera3D camera = 
	{
		{0,0,0}, // position
		{0,0,0}, // target
		{0,1,0}, // up
		90,      // fov
		CAMERA_PERSPECTIVE,
	};
	
	float nearClip;
	float farClip;
	
	// horizontal and vertical
	// camera sensitivity
	float HSENS = 0.1;
	float VSENS = 0.1;
	
	// third person camera
	int8 orbit = false;
	float orbitDist = 1; // distance from camera to orbit point
	vec3 orbitOffset = {0,0,0}; // distance from camera target to orbit point
	
	vec3 anchor = {0,0,0}; // pov-agnostic position of camera
	
	// in radians
	float pitch = 0;
	float   yaw = 0;
	float  roll = 0;
	
	void init()
	{
		anchor = {0,0,0};
		
		SetCameraMode(camera, CAMERA_CUSTOM);
		SetCameraMoveControls(0, 0, 0, 0, 0, 0);
		
		nearClip = 0.01;
		farClip = 1000;
		
		refresh();
	}
	
	void update()
	{
		if (!active or !process) return;
		
		vec2 rt = GetMouseDelta();
		rt = {RAD(rt.x) * -HSENS, RAD(rt.y) * VSENS};
		rotate(rt.y, rt.x, 0);
	}
	
	// -------------------------
	// camera manipulation
	// ------------------------
	
	void refresh()
	{
		Quaternion rotation = QuaternionFromEuler(pitch, yaw, roll);
		
		// position + looking direction
		camera.target = Vector3Add(
			anchor,
			Vector3RotateByQuaternion(
				{0,0,1}, rotation));
		
		if (orbit)
		{
			// position - looking direction * orbit distance
			camera.position = Vector3Subtract(
				anchor,
				Vector3RotateByQuaternion(
					{0,0,orbitDist * orbit}, rotation));
			camera.position = Vector3Add(camera.position, orbitOffset);
		}
		else camera.position = anchor;
	}
	
	void rotate(float p, float y, float r)
	{
		pitch = clamp((float)-1.55, pitch + p, (float)1.55);
		yaw = fmod(yaw + y, 2*M_PI);
		roll = fmod(roll + r, 2*M_PI);
		
		pitch = clamp((float)-1.55, pitch, (float)1.55);
		
		refresh();
	}
};
*/