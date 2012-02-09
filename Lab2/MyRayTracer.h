#ifndef OSU_H
#define OSU_H
#include "OSUInventor.h"
#endif
#ifndef LINUX
#include <ostream.h>
#else
#include <iostream>
using namespace std;
#endif
#include <Inventor/SbLinear.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoDirectionalLight.h>

#define PI 3.1415926536
#define ZERO 1e-6
#define FAR 1e10
#define EPSLON 0.0001
class MyRayTracer{
        public:
//        int length;
//        SbVec3f center;
//        SbVec3f center_new;
//        SbVec3f center_min;
//        SbVec3f *point_intersect;
//        SbVec3f distance;
//        float distance_length;
//        float distance_length_min;
//        float radius;
//        OSUObjectData *object;
//        OSUObjectData *closest_object;


//        float r, g, b;
//        SoType shape_type;
//        SbVec3f scale_vector;
//        SbVec3f point_on_sphere;
//        SbVec3f normal;
//        int is_intersect;
//        float z_component;

        /* Light */
        SoLight *light;
        SoType light_type; 
        SbVec3f light_color;
        SbVec3f ambient_color;
        SbVec3f diffuse_color;
        SbVec3f specular_color;        
        float Kd;//diffuse coefficient
        float light_intensity;
        SbVec3f light_location;
        SbVec3f light_vector;
        SbVec3f light_direction;



        MyRayTracer(OSUInventorScene *scene);
        int sphere_intersect(SbVec3f ray, SbVec3f eye, SbSphere sphere, SbVec3f *point_intersect);
        SbVec3f* rt(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbVec3f *color);

	int is_in_shadow(SbVec3f light, SbVec3f source, OSUInventorScene *scene, SbMatrix *transform_list);
};

