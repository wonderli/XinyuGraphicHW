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
#define ZERO 1e-6
#define FAR 1e10
void usage_error();
void set_object(OSUInventorScene *scene, SbMatrix *transform_list);
int sphere_intersect(SbVec3f ray, SbVec3f eye, SbSphere sphere, SbVec3f *point_intersect);
void ray_trace(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbColor *shadeColor);
int main(int argc, char **argv) {
	if (argc < 5)
		usage_error();
	SoDB::init();
        int length;
	OSUInventorScene *scene = new OSUInventorScene(argv[1]);
        length = scene->Objects.getLength();
        SbMatrix *transform_list = new SbMatrix[length];
	/*Set up camera*/
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
        set_object(scene, transform_list);//set up object list
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
        /*Calculation Resolution*/
        int yres = atoi(argv[3]);
        int xres = (int)(yres*(camera->aspectRatio.getValue()));
        int d = 1;//d is 1

        float pixel_height;
        float pixel_width;
        
        SbVec3f upperleft_corner, pixel_center, scanline_start;
        if (camera_type == SoPerspectiveCamera::getClassTypeId()) {
                pixel_height = 2 * tan(height_angle/2) * d /yres; //Calculate H, pixel height H/Y
                pixel_width = pixel_height * yres * camera_aspect_ratio / xres; // Calculate pixel width W/X
        }
//        pixel_center = eye - n * d;
//        upperleft_corner = pixel_center - (xres/2) * pixel_width * u + (yres/2) * pixel_height *v;
        upperleft_corner = eye - n - (xres/2)*pixel_width*u + (yres/2)*pixel_height*v;
        pixel_center = upperleft_corner + pixel_width/2*u - pixel_height/2*v;
        //scanline_start.setValue(pixel_center[0], pixel_center[1], pixel_center[2]);
        SbVec3f current, ray;
        SbColor *color = new SbColor();
 //       scanline_start = upperleft_corner - pixel_height *v;
        scanline_start.setValue(pixel_center[0], pixel_center[1], pixel_center[2]);


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
        float r, g, b;

       // float r, g, b;
        //iterate
        for(scanline = 0; scanline < yres; scanline++) {
                current = scanline_start;

                for (pixel = 0; pixel < xres; pixel++) {
                        ray = current - eye;
                        ray.normalize();
                        ray_trace(ray, eye, scene, transform_list, color);
                        r = (*color)[0];
                        g = (*color)[1];
                        b = (*color)[2];
                        fp << (int)r << ' ' << (int)g << ' ' <<(int)b <<endl;
                        current += pixel_width*u;
                }
                scanline_start -= pixel_height*v;
        }
        fp.close();
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
                //OSUObjectData *object = (OSUObjectData *)scene->Objects[i];
                object = (OSUObjectData *)scene->Objects[i];
                if(!object->Check()) {
                        cerr << "Error in OSUObjectData for object" << i << endl;
                        exit(0);
                }
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
        b = 2 * d.dot(ray);
//        c = d.dot(eye - sphere_center) - r * r;
        c = d.dot(d) - r * r;
        discriminant = b * b - 4 * a * c;
        if(discriminant > ZERO) {
//                root_1 = (-b + sqrt(discriminant))/(2*a);
//                root_2 = (-b - sqrt(discriminant))/(2*a);
                root_1 = (-b - sqrt(discriminant))/(2*a);
                root_2 = (-b + sqrt(discriminant))/(2*a);
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
        else{
                is_intersect = -1;
        }

        return is_intersect;
}

void ray_trace(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbColor *color) {
        int length = scene->Objects.getLength();
        int i;
        SbVec3f center(0, 0, 0);
        SbVec3f center_new(0, 0, 0);
        SbVec3f center_min(0, 0, 0);
        SbVec3f *point_intersect = new SbVec3f();
        SbVec3f distance;
        float distance_length;
        float distance_length_min = FAR;
        //float radius;
        float radius = 1;
        OSUObjectData *object = new OSUObjectData();
        OSUObjectData *closest_object = new OSUObjectData();
//        OSUObjectData *object = new OSUObjectData;
//        OSUObjectData *closest_object = new OSUObjectData;
        float r, g, b;
        SoType shape_type;
        SbVec3f scale_vector;
        SbVec3f point_on_sphere;
        SbVec3f normal;
        int is_intersect = -1;
        float z_component;

        for (i = 0; i < length; i++) {
                object = (OSUObjectData *)scene->Objects[i];
                shape_type = object->shape->getTypeId(); //Get Object ID

                if(shape_type == SoSphere::getClassTypeId()) {
                        center.setValue(0, 0, 0);
                        transform_list[i].multVecMatrix(center, center_new);
                        scale_vector = object->transformation->scaleFactor.getValue();
                        radius = scale_vector[0];
                        SbSphere *sphere = new SbSphere(center_new, radius);
                        is_intersect = sphere_intersect(ray, eye,*sphere, point_intersect);
                        if(is_intersect == 1) {
                                distance = eye - *point_intersect;
                                distance_length = distance.length();
                        } else
                        distance_length = FAR;

                        if(distance_length < distance_length_min) {
                                closest_object = object;
                                distance_length_min = distance_length;
                                point_on_sphere = *point_intersect;
                                center_min = center_new;
                        }
                }
        }
        if(distance_length_min < FAR) {
                normal = point_on_sphere - center_min;
                normal.normalize();
                z_component = normal[2];
                if (z_component > 0 && z_component < 0.3){
                        z_component = 0.3;
                }else if (z_component < 0){
                        z_component = 0;
                }

                r = closest_object->material->diffuseColor[0][0];
                g = closest_object->material->diffuseColor[0][1];
                b = closest_object->material->diffuseColor[0][2];

                r *= z_component;
                g *= z_component;
                b *= z_component;

                r *= 255;
                g *= 255;
                b *= 255;
        } else {
                r = 0;
                g = 0;
                b = 0;
        }
        (*color).setValue(r, g, b);
}
