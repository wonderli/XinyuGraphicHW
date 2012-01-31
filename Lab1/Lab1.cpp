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

#include <Inventor/SbLinear.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoPointLight.h>

#include "OSUInventor.h"
#define PI 3.1415926536
#define ZERO 1e-4
void usage_error();
void set_object(OSUInventorScene *scene, SbMatrix *transform_list);
int sphere_intersect(SbVec3f ray, SbVec3f eye, SbSphere sphere, SbVec3f *point_intersect);
int main(int argc, char **argv) {
	if (argc != 4)
		usage_error();
	SoDB::init();
	OSUInventorScene *scene = new OSUInventorScene(argv[1]);
	/*Set up camera*/
	SoCamera *camera = NULL;
        SbVec3f camera_position(0,0,1); //inital camera_position
        SbRotation camera_orientation(0,0,1,0); //initial camera_orientation
        SbVec3f camera_rotation_axis;
        SbVec3f camera_direction;
        SbVec3f camera_view_up;
        float filed_view_angle = PI/4; //45 degree
        float camera_roation_angle;
        float camera_aspect_ratio = 1.0;
        SoType camera_type;
        /* 
        * If camera is not defined, use the following default:
        *         camera position = (0,0,1)
        *         camera view up vector = (0,1,0)
        *         camera looks at (0,0,0)
        *         camera aspect ratio = 1.0 
        *         camera field of view angle = 45 degree
        * This is perspective camera.
        */
        if (scene->Camera == NULL) {
                camera = new SoPerspectiveCamera();
                camera_type =  SoPerspectiveCamera::getClassTypeId();
        } else {
                camera = scene->Camera;
                camera_position = camera->position.getValue();
                camera_orientation = camera->orientation.getValue();
                camera_orientation.getValue(camera_rotation_axis, camera_roation_angle);
                camera_aspect_ratio = camera->aspectRatio.getValue();
                camera_orientation.multVec(SbVec3f(0, 0, -1), camera_direction);
                camera_orientation.multVec(SbVec3f(0, 1, 0), camera_view_up);
        }
        SbVec3f eye, COI,n, v, u;
        eye.setValue(camera_position[0], camera_position[1], camera_position[2]);
        camera_direction.normalize();
        n.setValue(camera_direction[0], camera_direction[1], camera_direction[2]);
        n.negate(); // n setting
        u.setValue(camera_view_up.cross(n).getValue());
        u.normalize();
        v.setValue(n.cross(u).getValue());
        v.normalize();
        /*Calculation Resolution*/
        int yres = atoi(argv[3]);
        int xres = (int)(yres*(camera->aspectRatio.getValue()));
        int d = 1;//d is 1

        float pixel_height;
        float pixel_width;
        
        SbVec3f upperleft_corner, pixel_center, scanline_start;
        if (camera_type == SoPerspectiveCamera::getClassTypeId()) {
                pixel_height = 2 * tan(filed_view_angle/2) * d /yres; //Calculate H, pixel height H/Y
                pixel_width = pixel_height * yres * camera_aspect_ratio / xres; // Calculate pixel width W/X
        }
        pixel_center = eye - n * d;
        upperleft_corner = pixel_center - (xres/2) * pixel_width * u + (yres/2) * pixel_height *v;
        //scanline_start.setValue(pixel_center[0], pixel_center[1], pixel_center[2]);
        SbVec3f current, ray;
        SbColor shadeColor;
        scanline_start = upperleft_corner - pixel_height *v;

        //Initialize the ppm file
        fstream fp;
        fp.open(argv[2], fstream::out);
        if(!fp.is_open()){
                cerr << "Error in writing file" << argv[2] <<endl;
                exit(0);
        }
        fp << "P3"<<endl;
        fp << "#" << argv[2] <<endl;
        fp << xres << ' ' << yres <<endl;
        fp << "255" << endl;
        //Finish initialize file
        //Call shader for each pixel
        int scanline, pixel;

       // float r, g, b;
        //iterate
        for(scanline = 0; scanline < yres; scanline++) {
                current = scanline_start;

                for (pixel = 0; pixel < xres; pixel++) {
                        ray = current - eye;
                        ray.normalize();
//                        raytrace();

                }
        }
	return 0;


}
void usage_error() {
	cerr << "Usage: rt <input.iv> <output.ppm> <xres> <yres>" << endl;
	exit(10);
}
void set_object(OSUInventorScene *scene, SbMatrix *transform_list) {
        OSUObjectData *object = NULL;
        int length;
        SbMatrix T, S, R;
        SoTransform *transformation = NULL;
        SbVec3f scale_vector;
        SbRotation rotation;
        SbVec3f rotation_axis;
        float rotation_angle;
        SbVec3f translation_vector;
        length = scene->Objects.getLength();
        int i = 0;
        if(!scene) {
                cerr << "Scene is empty!" <<endl;
                exit(0);

        } 
        if(!transform_list) {
                cerr << "Tansform List not normal" << endl;
                exit(0);
        }
        for (i = 0; i < length; i++){
                object = (OSUObjectData *)scene->Objects[i];
                transformation = object->transformation;
                translation_vector = transformation->translation.getValue();
                scale_vector = transformation->scaleFactor.getValue();
                rotation = transformation->rotation.getValue();
                rotation.getValue(rotation_axis, rotation_angle);

                T.makeIdentity();
                S.makeIdentity();
                R.makeIdentity();

                T.setTranslate(translation_vector);
                S.setTranslate(scale_vector);
                R.setRotate(rotation);

                transform_list[i].makeIdentity();
                transform_list[i].multRight(S);
                transform_list[i].multRight(R);
                transform_list[i].multRight(T);
        }
}



int sphere_intersect(SbVec3f ray, SbVec3f eye, SbSphere sphere, SbVec3f *point_intersect) {
        float r = sphere.getRadius();
        SbVec3f sphere_center = sphere.getCenter();
        float a, b, c, discriminant;
        float root_1, root_2;
        float root;
        SbVec3f d = eye - sphere_center;
        int is_intersect = -1;
        a = 1;
        b = 2 * d.dot(eye - sphere_center);
        c = d.dot(eye - sphere_center) - r * r;
        discriminant = b * b - 4*a*c;
        if(discriminant > ZERO) {
                root_1 = (-b + sqrt(discriminant))/(2*a);
                root_2 = (-b - sqrt(discriminant))/(2*a);
                if(root_1 > ZERO) {
                        root = root_1;
                        is_intersect = 1;
                        *point_intersect = eye + ray * root;
                }
                else if(root_2 > ZERO) {
                        root = root_2;
                        is_intersect = 1;
                        *point_intersect = eye + ray * root;
                }
                else
                is_intersect = -1;
        }
        else 
        is_intersect = -1;
        return is_intersect;
}

