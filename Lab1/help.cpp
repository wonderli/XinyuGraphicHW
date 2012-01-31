// lab1.C  # CSE 681 

// argv[1]: OpenInventor .iv file 
// argv[2]: Out put .ppm file
// argv[3]: number of pixels per row

// Ying Wei

// general include files
#ifdef LINUX
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
#else
#include <iostream.h>
#include <vector.h>
#endif

#include <math.h>

// OpenInventor include files
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoMaterial.h> 

// OSU Inventor include files
#include "OSUInventor.h"

// global function declaration
void setObject(OSUInventorScene *scene, SbMatrix 	*TransformList);
void usage_error();
void shade(OSUInventorScene *scene, SbMatrix *TransformList, SbVec3f Eye, SbVec3f ray, SbColor *shadeColor);
bool sphereIntersect(SbVec3f Eye, SbVec3f ray, SbSphere shape, SbVec3f *PointXn);

#define 	PI	3.141592654
	
int main(int argc, char **argv) 
{
	// check for the right number of parameters
	if (argc != 4)
    		usage_error();

  	SoDB::init();
  	OSUInventorScene *scene = new OSUInventorScene(argv[1]);  

  	int len = scene->Objects.getLength();
  	SbMatrix *TransformList = new SbMatrix[len];
  	
  	//================================================================================================================
  	// Set list of objects
	setObject(scene, TransformList);
	
	//================================================================================================================
	// Set camera; only perspective camera is considered
	SoCamera 	*camera = NULL;		
	SbVec3f 	camera_position(0,0,1);
	SbRotation 	camera_orientation(0,0,1,0);
	float		heightAngle = PI/4.0;
	float		aspectRatio = 1.0;
	SoType 		camera_type;
    	
	if (scene->Camera == NULL){
		camera = new SoPerspectiveCamera;	// If no camera detected
		camera_type = SoPerspectiveCamera::getClassTypeId();
	}
	else {
		camera = scene->Camera;
  		camera_position = camera->position.getValue();
  		camera_orientation = camera->orientation.getValue();
		SbVec3f 	camera_rotation_axis;
    	float	camera_rotation_angle;
    		
    	camera_orientation.getValue(camera_rotation_axis, camera_rotation_angle);
  		aspectRatio = camera->aspectRatio.getValue();
  		camera_type = camera->getTypeId();
    		
  		if (camera_type == SoPerspectiveCamera::getClassTypeId()) 
		{	// camera is a perspective camera
     		SoPerspectiveCamera * perspective_camera = (SoPerspectiveCamera *) camera;
  			heightAngle = perspective_camera->heightAngle.getValue();
  		}
  	}
  	
  	// calculate camera direction and camera up direction
    SbVec3f camera_direction, camera_up;
    camera_orientation.multVec(SbVec3f(0, 0, -1), camera_direction);
    camera_orientation.multVec(SbVec3f(0, 1, 0), camera_up);
    	    	
	SbVec3f	Eye, v, n, u;	// these vectors are currently in world coordinate
	
	// Eye, the camera position
	Eye.setValue(camera_position[0],camera_position[1],camera_position[2]);
	// n, the camera direction
	camera_direction.normalize();
	n.setValue(camera_direction[0],camera_direction[1],camera_direction[2]);
	n.negate();
	
  	// v, the up vector
  	camera_up.normalize();
  	u.setValue(camera_up.cross(n).getValue());
  	u.normalize();
  	v.setValue(n.cross(u).getValue());
  	v.normalize();	
	
	//================================================================================================================
	// Resoloution Calculations
	int 	yres = atoi(argv[3]);
	int 	xres=(int)(yres*(camera->aspectRatio.getValue()));
	float 	pixelHeight, 	pixelWidth;
	float 	N = 1; 		// this is in camera space
  	SbVec3f upperLeftCorner,pixelCenter,scanlineStart;
  	
  	// if camera is a perspective camera
	if (camera_type == SoPerspectiveCamera::getClassTypeId()) {
		pixelHeight= 2 * tan(heightAngle/2) * N / yres; 
		pixelWidth = yres * pixelHeight * aspectRatio / xres ;
	}
  	
  	upperLeftCorner	= Eye - n - (xres/2)*pixelWidth*u + (yres/2)*pixelHeight*v;
  	pixelCenter 	= upperLeftCorner + pixelWidth/2*u - pixelHeight/2*v;
  	scanlineStart.setValue(pixelCenter[0],pixelCenter[1],pixelCenter[2]);
  	
	//================================================================================================================
	// Initialize the out.ppm file for output
  	fstream fp;
  	fp.open (argv[2],fstream::out);
  	
  	if(!fp.is_open()) {
  		cerr << "error during writing file " << argv[2] << '.' << endl;
  		exit(20);
  	}
  	
  	fp	<< "P3" << endl;
  	fp	<< "# "	<< argv[2] << endl;
  	fp	<< xres << ' ' << yres << endl;
	fp	<< "255" << endl;
  		  	  	
  	//================================================================================================================
  	// Call a shader for each pixel
	int 		sl, p;
	SbVec3f    	cur, ray;
	SbColor		*shadeColor = new SbColor;
	float		r, g, b;
		
	 //Loop through each scanline
  	for(sl = 0; sl < yres; sl++)
  	{ 
  		cur = scanlineStart;
  		
  		//Loop through each pixel in the scan line
      		for(p=0;p<xres;p=p++)
		{	
		  	//Form ray from eye through pixel: ray = pixelcenter-eye
  			ray = cur - Eye;		// vector				
		  	ray.normalize();	// normalized direction
		  			  	
		  	// Shading procedure: return to *shadeColor the resulting color of the pixel
		  	shade(scene, TransformList, Eye, ray, shadeColor); 
		  	
		  	// store the resulting color to the output file
		  	r	= (*shadeColor)[0];
		  	g	= (*shadeColor)[1];
		  	b 	= (*shadeColor)[2];
		  			  	
		  	fp << (int)r << ' ' << (int)g << ' ' << (int)b << endl;
		  	
		  	// Increment current pixel center to the next pixel 
		  	cur += pixelWidth*u;	 
		  	
	 	}//pixel loop
	 		 	
	 	// Increment current scanline to the next line
		scanlineStart -= pixelHeight*v;
		
     	}//scanline loop

	
  	// Close the output file	
  	fp.close();
  	//================================================================================================================
  	
  return(1);
}

void usage_error()
{
  cerr 	<< "Usage: lab1 input_file_name output_file_name pixel_width)" << endl;
  exit(10);
}

/*
 * setObject: 
 * calculate the transform matrix of each object in the current scene, and store the matrix in 
*/
void setObject(OSUInventorScene *scene, SbMatrix 	*TransformList)
{	
  	SbMatrix 		T,S,R;	// composite transform
  	SoTransform 	*transformation = NULL;
  	SbVec3f 		scale_vector;
  	SbRotation 		rotation;
  	SbVec3f 		rotation_axis;
    float 			rotation_angle;
    SbVec3f 		translation_vector;
    int 			len = scene->Objects.getLength();
    	
  	if (!scene)	{
  		cerr << "scene is empty" << endl;
  		exit(20);
  	}
  	
  	if (!TransformList ){
  		cerr << "TranformList not normal" << endl;
  		exit(20);
  	} 
  	
 	
  	for (int i = 0; i < len; i++) 
	{
		OSUObjectData 	*obj = (OSUObjectData *)scene->Objects[i];

    	if (!obj->Check()) 
		{
  			cerr 	<< "Error detected in OSUObjectData for object " << i << "." 	<< endl;	
  			exit(20);
		};
	
		// object transformation
		transformation 	= obj->transformation;
		translation_vector 	= transformation->translation.getValue();
		scale_vector 	= transformation->scaleFactor.getValue();
		rotation 		= transformation->rotation.getValue();    		
		rotation.getValue(rotation_axis, rotation_angle);
	
		// translate, scale, rotate, final
		T.makeIdentity();
		S.makeIdentity();
		R.makeIdentity();
	
		T.setTranslate(translation_vector);
   		S.setScale(scale_vector);
   		R.setRotate(rotation);
	
		TransformList[i].makeIdentity();
		TransformList[i].multRight(S);
		TransformList[i].multRight(R);
		TransformList[i].multRight(T);
		
  	}// end of setting object list  
}



// The shader: calculates the color that a pixel should display
// return the color in *shadeColor
void shade(	OSUInventorScene *scene, SbMatrix *TransformList,
		SbVec3f Eye, SbVec3f ray, SbColor *shadeColor)
{
	#define 	FARAWAY 	1e10

	// declare variables
  	bool 	isIntersect = false;
  	int 	i, len = scene->Objects.getLength();
  	float	radius = 1, distance, distanceMin = FARAWAY, r, g, b;
  	SbVec3f 	distanceVec, scale_vector;
	SoType  	shapeType;
	SbVec3f 	center(0,0,0), newCenter(0,0,0), MinCenter(0,0,0);
	SbVec3f	rPointXn, *PointXn = new SbVec3f, normal;
	OSUObjectData 	*obj = new OSUObjectData, *closestObject = new OSUObjectData;
	
    	
	//Check intersection of ray with each object
  	for (i = 0; i < len; i++) 
	{  		
   		obj = (OSUObjectData *)scene->Objects[i];
   		shapeType = obj->shape->getTypeId();
	   	
   		// only process shapes that are spheres; other shapes currently ignored
   		if(shapeType == SoSphere::getClassTypeId())
		{
	
   			// use object to world transform of sphere to transform the center	   	
   			center.setValue(0,0,0); 	   		
			TransformList[i].multVecMatrix(center,newCenter);
			scale_vector = obj->transformation->scaleFactor.getValue();		
			radius  = scale_vector[0];		// scale radius

     		SbSphere *shapeptr = new SbSphere(newCenter,radius);
     		
        	//PointXn is the world space intersection point
        	isIntersect = sphereIntersect(Eye, ray, *shapeptr, PointXn);
        
        	if(isIntersect){
      			distanceVec = Eye - *PointXn;
      			distance = distanceVec.length();
			}
			else 
     			distance = FARAWAY;
 
			if(distance < distanceMin)
			{
     			closestObject = obj;
      			distanceMin = distance;	     
      			rPointXn = *PointXn;
      			MinCenter = newCenter;
    		}	    		
   		}// end: if shape is sphere	   	
	}//for	

	if(distanceMin < FARAWAY)
	{ 
		// actually hit something 
		//Calculate color for that pixel 
		normal = rPointXn - MinCenter;
		normal.normalize();
		float zComponent = normal[2];
		
		if (zComponent >0 && zComponent < 0.3)
			zComponent = 0.3;
		else if (zComponent < 0)
			zComponent = 0;

		r = closestObject->material->diffuseColor[0][0];
		g = closestObject->material->diffuseColor[0][1];
		b = closestObject->material->diffuseColor[0][2];

		r *= zComponent;
		g *= zComponent;
		b *= zComponent;

		r *= 255;
		g *= 255;
		b *= 255;
		
	}
	else
	{
		//hardwired background color
		r = 0;
		g = 0;
		b = 0;
	}	
	
	(*shadeColor).setValue(r,g,b);
}


/*************************************************************************************************************
sphereIntersect: calculated in the world coordinate
Eye: 		eye position
ray: 		vector shooting from eye to the center of each pixel
shapeptr: 	the object sphere
PointXn:	the intersection point coordinate, if there is any
return type: bool
			true if the ray intersects the sphere
*************************************************************************************************************/

bool sphereIntersect(SbVec3f Eye, SbVec3f ray, SbSphere shape, SbVec3f *PointXn)
{
	float 	radius = shape.getRadius();
	SbVec3f   center = shape.getCenter(), temp;
	float 	a, b, c, discriminant, root1, root2, root;
	bool	ret = false;
	
	temp = Eye - center;
	a = 1;
	b = 2 * temp.dot(ray);
	c = temp.dot(temp) - radius*radius;
	discriminant = b*b-4*a*c;

	if (discriminant > 1e-5)
	{
		root1 = (-b - sqrt(discriminant))/(2*a);
		root2 = (-b + sqrt(discriminant))/(2*a);

		if(root1 > 1e-5){
			root = root1;
			ret = true;
			*PointXn = Eye + ray*root;
		}
		else if (root2 > 1e-5){
			root = root2;
			ret = true;
			*PointXn = Eye + ray*root;
		}
		else
			ret = false;
	}
	else
		ret = false;

	return ret;	
}



