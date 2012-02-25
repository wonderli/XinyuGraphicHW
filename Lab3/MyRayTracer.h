#ifndef OSU_H
#define OSU_H
#include "OSUInventor.h"
#endif
#ifdef LINUX
#include <iostream>
using namespace std;
#else
#include <ostream.h>
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
#define ZERO 0.0001
#define FAR 1e10
#define EPSLON 0.001
#define MAXRECURSION 3
#define True 1
#define False -1
#define RAY_OUTSIDE 1
#define RAY_INSIDE 0
class MyRayTracer{
        public:

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
        void rt(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbVec3f *color, int recursion_depth, int shadow_on, int reflection_on, int refraction_on, int ray_location);

	int is_in_shadow(SbVec3f intersect_point, SbVec3f light, SbVec3f light_location, OSUInventorScene *scene, SbMatrix *transform_list);

        int refract(SbVec3f d, SbVec3f n, float Eta, SbVec3f *t);

	float cube_intersect(SbVec3f ray, SbVec3f eye, SoCube *cube, SbMatrix transform_matrix, SbVec3f &point, SbVec3f &inter_normal);
};
