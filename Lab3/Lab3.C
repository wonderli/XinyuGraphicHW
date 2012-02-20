#ifdef LINUX
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
#else
#include <iostream.h>
#include <vector.h>
#include <fstream.h>
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
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoDirectionalLight.h>

#ifndef OSU_H
#define OSU_H
#include "OSUInventor.h"
#endif
#include "MyScene.h"
#include "MyRayTracer.h"
#define PI 3.1415926536
#define ZERO 1e-6
#define FAR 1e10
/*
* Name: usage_error
* Argument: void
* Return value: void
* Usage: Show the program usage to the user.
*/

void usage_error() {
	cerr << "Usage: rt <input.iv> <output.ppm> <xres> <yres>" << endl;
	exit(10);
}
int main(int argc, char **argv) {
	if (argc < 7)
		usage_error();
	SoDB::init();
        int length;
	OSUInventorScene *scene = new OSUInventorScene(argv[1]);
//        length = scene->Objects.getLength();
//        SbMatrix *transform_list = new SbMatrix[length];
	MyScene *my_scene = new MyScene(scene);
        SbMatrix *transform_list = my_scene->set_object();

	/* Set up camera */
	SoCamera *camera = NULL;
        SbVec3f camera_position(0,0,1); //inital camera_position
        SbRotation camera_orientation(0,0,1,0); //initial camera_orientation
        SbVec3f camera_rotation_axis;
        SbVec3f camera_direction;
        SbVec3f camera_view_up;
        float height_angle = PI/4; //45 degree
        float camera_roation_angle;
        float camera_aspect_ratio = 1.0;
        SoType camera_type;
//        set_object(scene, transform_list);//set up object list
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
                camera_type = camera->getTypeId();
                if (camera_type == SoPerspectiveCamera::getClassTypeId()) {
                        SoPerspectiveCamera *perspective_camera = (SoPerspectiveCamera *)camera;
                        height_angle = perspective_camera->heightAngle.getValue();
                }
        }
        camera_orientation.multVec(SbVec3f(0, 0, -1), camera_direction);
        camera_orientation.multVec(SbVec3f(0, 1, 0), camera_view_up);

        /* Set up eye, n, v, u */
        SbVec3f eye, COI,n, v, u;
        eye.setValue(camera_position[0], camera_position[1], camera_position[2]);
        camera_direction.normalize();
        n.setValue(camera_direction[0], camera_direction[1], camera_direction[2]);
        n.negate(); // n setting
        camera_view_up.normalize();
        u.setValue(camera_view_up.cross(n).getValue());
        u.normalize();
        v.setValue(n.cross(u).getValue());
        v.normalize();

        /* Calculation Resolution */
        int xres = atoi(argv[3]);
        int yres = atoi(argv[4]);
        
        if(xres != (int)(yres*(camera->aspectRatio.getValue()))) {
                cout << "yres and xres value should follow the aspect ratio" << endl;
                yres = xres/(camera->aspectRatio.getValue());
                cout << "Now the yres value is " << yres << endl;
        }

        //int xres = (int)(yres*(camera->aspectRatio.getValue()));

        /* Image plane setup */
        int d = 1;//d is 1

        float pixel_height;
        float pixel_width;
        
        SbVec3f upperleft_corner, pixel_center, scanline_start;
        if (camera_type == SoPerspectiveCamera::getClassTypeId()) {
                pixel_height = 2 * tan(height_angle/2) * d /yres; //Calculate H, pixel height H/Y
                pixel_width = pixel_height * yres * camera_aspect_ratio / xres; // Calculate pixel width W/X
        }

        pixel_center = eye - n * d;
        upperleft_corner = pixel_center - (xres/2) * pixel_width * u + (yres/2) * pixel_height *v;
//        upperleft_corner = eye - n - (xres/2)*pixel_width*u + (yres/2)*pixel_height*v;
//        pixel_center = upperleft_corner + pixel_width/2*u - pixel_height/2*v;


        /*Initialize the ppm file
        * PPM format is like the following:
        * P3
        * #comment
        * xresolution yresolution
        * 255
        * The maximu pixel value
        */

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
        /* Finish initialize PPM file */

        /* Begin ray tracing */
        SbVec3f current, ray;
        int scanline, pixel;
        //SbColor *color = new SbColor();
        SbVec3f *color = new SbVec3f(0, 0, 0);
        float r, g, b;
        int recursion_depth = 0;
        int shadow_on = atoi(argv[5]);
        int reflection_on = atoi(argv[6]);

        MyRayTracer *my_rt = new MyRayTracer(scene);
        /* Set scanline start begin at the left corner */
        scanline_start.setValue(upperleft_corner[0], upperleft_corner[1], upperleft_corner[2]);
        /* Begin to do the iterate */
        for(scanline = 0; scanline < yres; scanline++) {
                current = scanline_start;

                for (pixel = 0; pixel < xres; pixel++) {
                        ray = current - eye;
                        ray.normalize();

//                        my_rt->rt //ray_trace(ray, eye, scene, transform_list, color);
                        int ray_location = RAY_OUTSIDE;                        
                        my_rt->rt(ray, eye, scene, transform_list, color, recursion_depth, shadow_on, reflection_on, ray_location);

                        r = (*color)[0];
                        g = (*color)[1];
                        b = (*color)[2];
//                        color->getValue(r, g, b);
                        r = r * 255;
                        g = g * 255;
                        b = b * 255;
                        if(r > 255) r = 255;
                        else if(r < 0) r = 0;
                        if(g > 255) g = 255;
                        else if(g < 0) g = 0;
                        if(b > 255) b = 255;
                        else if(b < 0) b = 0;

                        /* Print to file*/
                        fp << (int)r << ' ' << (int)g << ' ' <<(int)b <<endl;
                        /* Move forward to next pixel*/
                        current += pixel_width*u;
                }

                scanline_start -= pixel_height*v;

        }
        fp.close();
	return 0;
}

