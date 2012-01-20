#ifdef LINUX
#include <iostream>
#include <vector>
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
void usage_error();
int main(int argc, char **argv) {
	if (argc != 2)
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
                camera_type = new SoPerspectiveCamera::getClassTypeId();
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
        n = eye.normalize().negate(); // n setting
        u = u.setValue(camera_view_up(cross(n).getValue)).normalize();
        v = v.setValue(n.cross(u).getValue).normalize();







	return 0;


}
void usage_error() {
	cerr << "Usage: rt <input.iv> <output.ppm> <xres> <yres>" << endl;
	exit(10);
}
