#include "MyRayTracer.h"
MyRayTracer::MyRayTracer(OSUInventorScene *scene) {
//	this->length = scene->Objects.getLength();
//	this->center.setValue(0, 0, 0);
//	this->center_new.setValue(0, 0, 0);
//	this->center_min.setValue(0, 0, 0);
//	this->point_intersect = new SbVec3f();
//	this->radius = 1;
//	this->object = new OSUObjectData();
//	this->closest_object = new OSUObjectData();
//        this->distance_length_min = FAR;

//	this->is_intersect = -1;
	this->light = NULL;
	this->light_color.setValue(0, 0, 0);
	this->ambient_color.setValue(0, 0, 0);
	this->diffuse_color.setValue(0, 0, 0);
	this->specular_color.setValue(0, 0, 0);
	this->light_intensity = 1;
}
int MyRayTracer::sphere_intersect(SbVec3f ray, SbVec3f eye, SbSphere sphere, SbVec3f *point_intersect) {
	float r = sphere.getRadius();
	SbVec3f sphere_center = sphere.getCenter();
	float a, b, c, discriminant;
	float root_1, root_2;
	float root;
	int is_intersect = -1;
	SbVec3f d = ray;
	SbVec3f eye_minus_center = eye - sphere_center;

	a = d.dot(d);
	b = 2 * d.dot(eye - sphere_center);
	c = eye_minus_center.dot(eye_minus_center) - r * r;

	discriminant = b * b - 4 * a * c;
	if(discriminant > ZERO) {
		root_1 = (-b - sqrt(discriminant))/(2*a);
		root_2 = (-b + sqrt(discriminant))/(2*a);
		if((root_1 > ZERO) && (root_2 > ZERO)) {
			if (root_1 < root_2) {
				root = root_1;
				is_intersect = 1;
				*point_intersect = eye + ray * root;
			} else {
				root = root_2;
				is_intersect = 1;
				*point_intersect = eye + ray * root;
			} 

		} else {
			is_intersect = -1;
		}
	}
	return is_intersect;
}
SbVec3f* MyRayTracer::rt(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbVec3f *color) {

	int i = 0;
	int length = scene->Objects.getLength();
	float radius;
	float r, g, b;
	float z_component;
	int is_intersect = -1;
        SoType shape_type;
        SbVec3f center(0, 0, 0);
        SbVec3f center_new(0, 0, 0);
        SbVec3f center_min(0, 0, 0);
        SbVec3f scale_vector;
        SbVec3f *point_intersect = new SbVec3f();
        SbVec3f distance;
        float distance_length;
        float distance_length_min = FAR;
        OSUObjectData *object = new OSUObjectData();
        OSUObjectData *closest_object = new OSUObjectData();
	SbVec3f point_on_sphere;
        SbVec3f normal;
        for (i = 0; i < length; i++) {
		object = (OSUObjectData *)scene->Objects[i];
		shape_type = object->shape->getTypeId();
		if(shape_type == SoSphere::getClassTypeId()) {
			center.setValue(0, 0, 0);
			transform_list[i].multVecMatrix(center, center_new);
			scale_vector = object->transformation->scaleFactor.getValue();
			radius = scale_vector[0];
			SbSphere *sphere = new SbSphere(center_new, radius);
			is_intersect = this->sphere_intersect(ray, eye, *sphere, point_intersect);
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
                /*
                r = closest_object->material->diffuseColor[0][0] * z_component * 255;
                g = closest_object->material->diffuseColor[0][1] * z_component * 255;
                b = closest_object->material->diffuseColor[0][2] * z_component * 255;
                */
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
        return color;


}
