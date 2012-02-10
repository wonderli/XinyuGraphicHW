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

/* 
* Name: sphere_intersect
* Argument: 
*          ray: ray vector shooting from eye to the center of each pixel
*          eye: eye position
*          sphere: sphere need to be tested
*          point_intersect: the intersection point coordinate.*
* Return value: 1 means intersect with the sphere, -1 mesans have no intersect with the sphere
* Usage:
*       Check the ray and sphere whether have intersection or not, if have, store the intersection point.
*/

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


/*
* Name: rt
* Argument:
*          ray: ray vector shooting from eye to the center of each pixel
*          eye: eye position
*          scene: scene info
*          transform_list: Transform list from set_object function
*          color: color info at each pixel
* Return vaule: void
* Usage:
*       Ray tracing main algorithm. Get the closest object intersection point, and set the corresponding color at that pixel.
*
*
*/
void MyRayTracer::rt(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbVec3f *color, int recursion_depth)
{

        int i = 0;
        int length = scene->Objects.getLength();
        float radius;
        float r, g, b;
        float z_component;
        int is_intersect = -1;
        int is_inshadow = -1;
        SoType shape_type;
        SbVec3f center(0, 0, 0);
        SbVec3f center_new(0, 0, 0);
        SbVec3f center_min(0, 0, 0);
        SbVec3f scale_vector;
        SbVec3f *point_intersect = new SbVec3f();
        SbVec3f *point_intersect_shadow = new SbVec3f();
        SbVec3f distance;
        float distance_length;
        float distance_length_min = FAR;
        OSUObjectData *object = new OSUObjectData();
        OSUObjectData *object_shadow = new OSUObjectData();
        OSUObjectData *closest_object = new OSUObjectData();
        SbVec3f point_on_sphere;
        SbVec3f normal;
        SoLight *light = NULL;
        SbVec3f light_location(0, 0, 0);
        //        SbVec3f light_intensity(0, 0, 0);
        float light_intensity = 0;        
        SbVec3f light_color(0, 0, 0);
        SoType light_type;
        SbVec3f ambient_color(0, 0, 0);
        SbVec3f diffuse_color(0, 0, 0);
        SbVec3f specular_color(0, 0, 0);
        SbVec3f light_vector(0, 0, 0);
        SbVec3f R(0, 0, 0);
        float color0 = 0;
        float color1 = 0 ;
        float color2 = 0;
//        if(recursion_depth > 0)
//        {
//        color->getValue(color0, color1, color2);
//        }
//        cout<<"THE COLOR IS " << color0 << " " << color1 << " " << color2 <<endl;
	int min_index = 0;
        float shininess_factor = 0;
        SbVec3f reflection_ray(0, 0, 0);
//        SbVec3f *reflect_color = NULL;
        float reflection_color0 = 0;
        float reflection_color1 = 0;
        float reflection_color2 = 0;

        for (i = 0; i < length; i++)
        {
                object = (OSUObjectData *)scene->Objects[i];
                shape_type = object->shape->getTypeId();
                if(shape_type == SoSphere::getClassTypeId()) 
                {
                        center.setValue(0, 0, 0);
                        transform_list[i].multVecMatrix(center, center_new);
                        scale_vector = object->transformation->scaleFactor.getValue();
                        radius = scale_vector[0];
                        SbSphere *sphere = new SbSphere(center_new, radius);
                        is_intersect = this->sphere_intersect(ray, eye, *sphere, point_intersect);
                        if(is_intersect == 1) 
                        {
                                distance = eye - *point_intersect;
                                distance_length = distance.length();
                        } else
                        distance_length = FAR;
                        if(distance_length < distance_length_min) 
                        {
                                closest_object = object;
                                distance_length_min = distance_length;
                                point_on_sphere = *point_intersect;
                                center_min = center_new;
				min_index = i;
                        }
                }
        }
        if(distance_length_min < FAR)
        {
                normal = point_on_sphere - center_min;
                normal.normalize();

                for(i = 0; i < length; i++) 
                {
                        light = (SoLight *)scene->Lights[i];
                        if(light != NULL)
                        {
                                if(light->on.getValue() == false)
                                {
                                        continue;
                                }
                                light_type = light->getTypeId();                               
                                if(light_type == SoPointLight::getClassTypeId())
                                {
                                        SoPointLight *point_light = (SoPointLight *)light;
                                        light_location = point_light->location.getValue();
                                        #ifdef DEBUG
//                                        cout << "THE LIGHT LOCATION IS " << " " << light_location[0] << " " << light_location[1] << " " << light_location[2] << endl;
                                        #endif
                                        light_intensity = point_light->intensity.getValue();                                
                                        light_color = point_light->color.getValue();                                
                                }
                                light_vector = point_on_sphere - light_location; 
                                light_vector.normalize();
                                light_vector.negate();
                                reflection_ray = (-2) * light_vector.dot(normal) * normal + light_vector;
                                float N_dot_L = normal.dot(light_vector);
                                SbVec3f V = ray;
                                V.normalize();
                                V.negate();
                                float V_dot_N = V.dot(normal);
                                R = 2 * N_dot_L * normal - light_vector;
                                R.normalize();
                                float V_dot_R = V.dot(R);
                                object = (OSUObjectData *)scene->Objects[min_index];
                                shininess_factor = object->material->shininess[0];

                                float ambient_color0 = object->material->ambientColor[0][0];
                                float ambient_color1 = object->material->ambientColor[0][1];
                                float ambient_color2 = object->material->ambientColor[0][2];
                                ambient_color.setValue(ambient_color0, ambient_color1, ambient_color2);
                                /* Compute reflective Ray*/
                                if (N_dot_L < 0)
                                {
                                        ambient_color.setValue(0, 0, 0);
                                        diffuse_color.setValue(0, 0, 0);
                                        specular_color.setValue(0, 0, 0);

                                }else 
                                {
                                        is_inshadow = is_in_shadow(point_on_sphere, light_vector,  light_location, scene, transform_list);                             

                                        if(is_inshadow == 1)
                                        {
                                                diffuse_color.setValue(0.1, 0.1, 0.1);
                                                specular_color.setValue(0, 0, 0);
                                        }
                                        else if(is_inshadow == -1)
                                        {

                                                float diffuse_color0 = object->material->diffuseColor[0][0];
                                                float diffuse_color1 = object->material->diffuseColor[0][1];
                                                float diffuse_color2 = object->material->diffuseColor[0][2];
                                                diffuse_color.setValue(diffuse_color0, diffuse_color1, diffuse_color2);

                                                float specular_color0 = object->material->specularColor[0][0];
                                                float specular_color1 = object->material->specularColor[0][1];
                                                float specular_color2 = object->material->specularColor[0][2];
                                                specular_color.setValue(specular_color0, specular_color1, specular_color2);
//                                                diffuse_color.setValue(0, 0, 0);
//                                                specular_color.setValue(0, 0, 0);

                                        }
                                }
                                              color0 += light_intensity * light_color[0] * ambient_color[0] + N_dot_L * diffuse_color[0] * light_intensity * light_color[0] + pow(V_dot_R, 20) * specular_color[0] * light_intensity * light_color[0];
                                        color1 += light_intensity * light_color[1] * ambient_color[1] + N_dot_L * diffuse_color[1] * light_intensity * light_color[1] + pow(V_dot_R, 20) *specular_color[1] * light_intensity * light_color[1];
                                        color2 += light_intensity * light_color[2] * ambient_color[2] + N_dot_L * diffuse_color[2] * light_intensity * light_color[2] + pow(V_dot_R, 20) *specular_color[2] * light_intensity * light_color[2];

//                                              color0 += light_intensity * light_color[0] * ambient_color[0] + N_dot_L * diffuse_color[0] * light_intensity * light_color[0] * ambient_color[0] + pow(V_dot_R, 20) * specular_color[0] * light_intensity * light_color[0];
//                                        color1 += light_intensity * light_color[1] * ambient_color[1] + N_dot_L * diffuse_color[1] * light_intensity * light_color[1] * ambient_color[1] + pow(V_dot_R, 20) *specular_color[1] * light_intensity * light_color[1];
//                                        color2 += light_intensity * light_color[2] * ambient_color[2] + N_dot_L * diffuse_color[2] * light_intensity * light_color[2] * ambient_color[2] + pow(V_dot_R, 20) *specular_color[2] * light_intensity * light_color[2];
//



                                        if(recursion_depth < MAXRECURSION)
                                        {
                                                if(shininess_factor > 0) 
                                                {       
                        //                                                        color0 = color0 * shininess_factor;
//                                                        color1 = color1 * shininess_factor;
//                                                        color2 = color2 * shininess_factor;
//                                                        color->setValue(color0, color1, color2);
                                                        SbVec3f reflection_ray_normal = reflection_ray;
                                                        reflection_ray_normal.normalize();
                                                        SbVec3f *reflection_color = new SbVec3f(0, 0, 0);
                                                        this->rt(reflection_ray_normal, point_on_sphere + EPSLON * reflection_ray_normal, scene, transform_list, reflection_color, recursion_depth + 1);
                                                        float reflection_color0 = 0;
                                                        float reflection_color1 = 0;
                                                        float reflection_color2 = 0;
                                                        reflection_color->getValue(reflection_color0, reflection_color1, reflection_color2);
                                                        color0 = color0 + shininess_factor * reflection_color0;
                                                        color1 = color1 + shininess_factor * reflection_color1;
                                                        color2 = color2 + shininess_factor * reflection_color2;
                                                }

                                        }




                }else 
                continue;

        }

              }
              else
              {
                        color0 = 0;
                        color1 = 0;
                        color2 = 0;
              }                

                (*color).setValue(color0, color1, color2);

}



int MyRayTracer::is_in_shadow(SbVec3f intersect_point, SbVec3f light, SbVec3f source, OSUInventorScene *scene, SbMatrix *transform_list)
{
        int in_shadow = -1;
        int i = 0;
        int length = scene->Objects.getLength();
        OSUObjectData *object = NULL;
        SbVec3f *point_intersect = new SbVec3f();
        SoType shape_type;
        SbVec3f center(0, 0, 0);
        SbVec3f center_new(0, 0, 0);
        SbVec3f center_min(0, 0, 0);
        SbVec3f scale_vector;
        float radius = 0;
        light.normalize();
        light.negate();
        SbVec3f P = intersect_point + EPSLON * light;
        SbVec3f Ray = source - P;


        int is_intersect = -1;
        for (i = 0; i < length; i++)
        {
                object = (OSUObjectData *)scene->Objects[i];
                shape_type = object->shape->getTypeId();
                if(shape_type == SoSphere::getClassTypeId()) 
                {
                        center.setValue(0, 0, 0);
                        transform_list[i].multVecMatrix(center, center_new);
                        scale_vector = object->transformation->scaleFactor.getValue();
                        radius = scale_vector[0];
                        SbSphere *sphere = new SbSphere(center_new, radius);
                        is_intersect = this->sphere_intersect(Ray, intersect_point, *sphere, point_intersect);
                        if(is_intersect == 1) 
                        {
                                in_shadow = 1;
                                break;
                        }
                }
        }

        return in_shadow;

}
