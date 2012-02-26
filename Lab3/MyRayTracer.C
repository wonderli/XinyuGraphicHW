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
*          recursion_depth: for calculating reflection
*          shadow_on: flag for shadow
*          reflection_on: flag for reflection
* Return vaule: void
* Usage:
*       Ray tracing main algorithm. Get the closest object intersection point, and set the corresponding color at that pixel.
*
*
*/

                void MyRayTracer::rt(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbVec3f *color, int recursion_depth, int shadow_on, int reflection_on, int refraction_on, int ray_location)
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
 //                       SbVec3f point_on_sphere;
                        SbVec3f point_on_object;
                        SbVec3f normal;
                        SbVec3f intersect_normal;
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
                        int min_index = 0;
                        float shininess_factor = 0;
                        SbVec3f reflection_ray(0, 0, 0);
                        float reflection_color0 = 0;
                        float reflection_color1 = 0;
                        float reflection_color2 = 0;
//                        float EPSLON = 1e-3;
                        float Eta_i = 1.0;
                        float Eta_t = 1.5;
                        float Eta = Eta_i/Eta_t;
                        float transparency_factor = 0;
                        float cosine_value = 0;
                        float R_Zero = 0;
                        float R_Value = 0;
//                        float reflection_color0 = 0;
//                        float reflection_color1 = 0;
//                        float reflection_color2 = 0;

                        float refraction_color0 = 0;
                        float refraction_color1 = 0;
                        float refraction_color2 = 0;
                        int ONLY_REFLECTION = 0;
//			float reflection_color0_total = 0;
//			float reflection_color1_total = 0;
//			float reflection_color2_total = 0;
                        float specular_color_transparency0 = 0;			
                        float specular_color_transparency1 = 0;			
                        float specular_color_transparency2 = 0;			

                        SbVec3f *refraction_ray = new SbVec3f(0, 0, 0);
			//SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     

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
                                                intersect_normal = *point_intersect - center_new;
                                                intersect_normal.normalize();

                                        } else
                                        distance_length = FAR;
                                       
                                }//end of sphere intersect  
				else if(shape_type == SoCube::getClassTypeId()) 
                                {
                                        SoCube *cube = (SoCube*)(object->shape);
                                        distance_length = this->cube_intersect(ray, eye, cube, transform_list[i], point_on_object, intersect_normal);
                                        cout<<"DISTANCE LENGTH"<<distance_length<<endl;
                                        *point_intersect = point_on_object; 
					intersect_normal.normalize();

                                }//end of cube intersect

                                if(distance_length < distance_length_min) 
                                {
                                        closest_object = object;
                                        distance_length_min = distance_length;
                                        point_on_object = *point_intersect;
                                        //center_min = center_new;
                                        normal = intersect_normal;
                                        min_index = i;
                                }


                        }
                if(distance_length_min < FAR)
                {
                        //normal = point_on_sphere - center_min;
                        //normal.normalize();
                        if(ray_location == RAY_INSIDE)
                        {
                                normal.negate();
                        }
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
                                                light_intensity = point_light->intensity.getValue();                                
                                                light_color = point_light->color.getValue();                                
                                        }
                                        light_vector = point_on_object - light_location; 
                                        light_vector.normalize();
                                        SbVec3f reflect_light_vector = light_vector;
                                        light_vector.negate();
                                        //reflection_ray = (-2) * light_vector.dot(normal) * normal + light_vector;
                                        float N_dot_L = normal.dot(light_vector);
                                        SbVec3f V = ray;
                                        V.normalize();
                                        V.negate();
                                        float V_dot_N = V.dot(normal);
                                        R = 2 * N_dot_L * normal - light_vector;
                                        R.normalize();
                                        float V_dot_R = V.dot(R);
                                        //object = (OSUObjectData *)scene->Objects[min_index];
                                        object = closest_object;
                                        shininess_factor = object->material->shininess[0];
                                        
                                        /*ambient color*/
					//if(object->material->ambientColor[0][0]!= NULL)
					if(object->material->ambientColor[0][0] >= 0)
					{
						float ambient_color0 = object->material->ambientColor[0][0];
						float ambient_color1 = object->material->ambientColor[0][1];
						float ambient_color2 = object->material->ambientColor[0][2];
						ambient_color.setValue(ambient_color0, ambient_color1, ambient_color2);
					}
					else
					{
						ambient_color.setValue(0, 0, 0);
					}

                                        if(shadow_on == 1)
                                        {
                                                /* color set to zero for back face light*/
                                                if (N_dot_L < 0)
                                                {
                                                        ambient_color.setValue(0, 0, 0);
                                                        //ambient_color.setValue(0.1, 0.1, 0.1);
                                                        diffuse_color.setValue(0, 0, 0);
                                                        specular_color.setValue(0, 0, 0);

                                                }else 
                                                {
                                                        is_inshadow = is_in_shadow(point_on_object, light_vector,  light_location, scene, transform_list);                             
                                                        /* in shadow */
                                                        if(is_inshadow == True)
                                                        {
                                                                ambient_color.setValue(0, 0, 0);
                                                                //ambient_color.setValue(0.1, 0.1, 0.1);
                                                                diffuse_color.setValue(0, 0, 0);
                                                                specular_color.setValue(0, 0, 0);
                                                        }
                                                        else if(is_inshadow == False)
                                                        {

                                                                float diffuse_color0 = object->material->diffuseColor[0][0];
                                                                float diffuse_color1 = object->material->diffuseColor[0][1];
                                                                float diffuse_color2 = object->material->diffuseColor[0][2];
                                                                diffuse_color.setValue(diffuse_color0, diffuse_color1, diffuse_color2);

                                                                float specular_color0 = object->material->specularColor[0][0];
                                                                float specular_color1 = object->material->specularColor[0][1];
                                                                float specular_color2 = object->material->specularColor[0][2];
                                                                specular_color.setValue(specular_color0, specular_color1, specular_color2);

                                                        }
                                                }

                                        }
                                        else if(shadow_on == 0)
                                        {
                                                if (N_dot_L < 0)
                                                {
                                                        ambient_color.setValue(0, 0, 0);
                                                        //ambient_color.setValue(0.1, 0.1, 0.1);
                                                        diffuse_color.setValue(0, 0, 0);
                                                        specular_color.setValue(0, 0, 0);

                                                }else 
                                                {

                                                        float diffuse_color0 = object->material->diffuseColor[0][0];
                                                        float diffuse_color1 = object->material->diffuseColor[0][1];
                                                        float diffuse_color2 = object->material->diffuseColor[0][2];
                                                        diffuse_color.setValue(diffuse_color0, diffuse_color1, diffuse_color2);

                                                        float specular_color0 = object->material->specularColor[0][0];
                                                        float specular_color1 = object->material->specularColor[0][1];
                                                        float specular_color2 = object->material->specularColor[0][2];
                                                        specular_color.setValue(specular_color0, specular_color1, specular_color2);
                                                }

                                        }

                                /* Color calculate*/
				if(refraction_on == True)
				{
					if(object->material->transparency[0] >= 0)
					{
						transparency_factor =  object->material->transparency[0];
					}
					if(transparency_factor > 0)
					{
						color0 += 0.2 * ambient_color[0] + N_dot_L * diffuse_color[0] * light_intensity * light_color[0];
						specular_color_transparency0 = pow(V_dot_R,50) * specular_color[0] * light_intensity * light_color[0];
						color1 += 0.2 * ambient_color[1] + N_dot_L * diffuse_color[1] * light_intensity * light_color[1];
						specular_color_transparency1 = pow(V_dot_R, 50) * specular_color[1] * light_intensity * light_color[1];
						color2 += 0.2 * ambient_color[2] + N_dot_L * diffuse_color[2] * light_intensity * light_color[2];
						specular_color_transparency2 = pow(V_dot_R, 50) * specular_color[2] * light_intensity * light_color[2];

					}
					else
					{
						color0 += 0.2 * ambient_color[0] + N_dot_L * diffuse_color[0] * light_intensity * light_color[0] + pow(V_dot_R, 50) * specular_color[0] * light_intensity * light_color[0];
						color1 += 0.2 * ambient_color[1] + N_dot_L * diffuse_color[1] * light_intensity * light_color[1] + pow(V_dot_R, 50) *specular_color[1] * light_intensity * light_color[1];
						color2 += 0.2 * ambient_color[2] + N_dot_L * diffuse_color[2] * light_intensity * light_color[2] + pow(V_dot_R, 50) *specular_color[2] * light_intensity * light_color[2];

					}
				}
				else
				{
					color0 += 0.2 * ambient_color[0] + N_dot_L * diffuse_color[0] * light_intensity * light_color[0] + pow(V_dot_R, 50) * specular_color[0] * light_intensity * light_color[0];
					color1 += 0.2 * ambient_color[1] + N_dot_L * diffuse_color[1] * light_intensity * light_color[1] + pow(V_dot_R, 50) *specular_color[1] * light_intensity * light_color[1];
					color2 += 0.2 * ambient_color[2] + N_dot_L * diffuse_color[2] * light_intensity * light_color[2] + pow(V_dot_R, 50) *specular_color[2] * light_intensity * light_color[2];

				}
                                if(shadow_on == 0)//For no shadow but still want specular light
                                {
                                        color0 += specular_color_transparency0;
                                        color1 += specular_color_transparency1;
                                        color2 += specular_color_transparency2;
                                }


                }else 
                continue;
        }


              }
              else
              {
                      color0 = 0.1;
                      color1 = 0.1;
                      color2 = 0.1;
//		      color0 = 0;
//                      color1 = 0;
//                      color2 = 0;

              }                

              /* Reflection */  

              if(reflection_on == 1)
              {
                      if(recursion_depth < MAXRECURSION)
                      {
                              if(shininess_factor > 0) 
                              {       

                                      reflection_ray = (-2) * ray.dot(normal) * normal + ray;
                                      SbVec3f reflection_ray_normal = reflection_ray;
                                      reflection_ray_normal.normalize();
                                      SbVec3f *reflection_color = new SbVec3f(0, 0, 0);                                      
                                      this->rt(reflection_ray_normal, point_on_object + EPSLON * reflection_ray_normal, scene, transform_list, reflection_color, recursion_depth + 1, shadow_on, reflection_on, refraction_on, 1);
                                                                            
                                      reflection_color->getValue(reflection_color0, reflection_color1, reflection_color2);
//                                      color0 = color0 + shininess_factor * reflection_color0;
//                                      color1 = color1 + shininess_factor * reflection_color1;
//                                      color2 = color2 + shininess_factor * reflection_color2;
                                      if(refraction_on == True)
                                      {
                                              if(object->material->transparency[0] > 0)
                                              {
                                                      transparency_factor =  object->material->transparency[0];
                                              }
                                              if(transparency_factor > 0)
                                              {
                                                      color0 = (color0 + shininess_factor * reflection_color0)*(1-transparency_factor) + specular_color_transparency0;
                                                      color1 = (color1 + shininess_factor * reflection_color1)*(1-transparency_factor) + specular_color_transparency1;
                                                      color2 = (color2 + shininess_factor * reflection_color2)*(1-transparency_factor) + specular_color_transparency2;

                                              }
                                              else
                                              { 
                                                      color0 = color0 + shininess_factor * reflection_color0;
                                                      color1 = color1 + shininess_factor * reflection_color1;
                                                      color2 = color2 + shininess_factor * reflection_color2;
                                              }
                                      }
                                      else
                                      {                                      
                                              color0 = color0 + shininess_factor * reflection_color0;
                                              color1 = color1 + shininess_factor * reflection_color1;
                                              color2 = color2 + shininess_factor * reflection_color2;
                                      }
                              }
                              if(refraction_on == True)
                              {
                                      if(object->material->transparency[0] > 0)
                                      {
                                              transparency_factor =  object->material->transparency[0];
                                              if(ray.dot(normal) < 0)
                                              {
                                                      this->refract(ray, normal, Eta, refraction_ray);
                                                      SbVec3f refraction_ray_normal = *refraction_ray;
                                                      refraction_ray_normal.normalize();
                                                      //float test0 = 0;
                                                      //float test1 = 0;
                                                      //float test2 = 0;
                                                      //point_on_object.getValue(test0, test1, test2);
                                                      //cout<<"POINT_ON_OBJECT"<<"("<<test0<<","<<test1<<","<<test2<<")"<<endl;
                                                      //SbVec3f test_point = point_on_object+EPSLON*refraction_ray_normal;
                                                      //test_point.getValue(test0, test1, test2);
                                                      //cout<<"POINT_REFRACTION_POINT"<<"("<<test0<<","<<test1<<","<<test2<<")"<<endl;
                                                     
                                                      SbVec3f *refraction_color = new SbVec3f(0, 0, 0); 
                                                      this->rt(refraction_ray_normal, point_on_object + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, recursion_depth + 1, shadow_on, reflection_on, refraction_on, RAY_OUTSIDE);
                                                      refraction_color->getValue(refraction_color0, refraction_color1, refraction_color2);
                                                      color0 = color0 + transparency_factor * refraction_color0;
                                                      color1 = color1 + transparency_factor * refraction_color1;
                                                      color2 = color2 + transparency_factor * refraction_color2;
                                                      //cosine_value = (-1) * normal.dot(ray);
                                              }
                                              else
                                              {
                                                      //SbVec3f normal_neg = normal;
                                                      //normal_neg.negate();
                                                      //if(True == this->refract(ray, normal_neg, 1/Eta, refraction_ray))
                                                      if(True == this->refract(ray, normal, 1/Eta, refraction_ray))
                                                      {
                                                              SbVec3f refraction_ray_normal = *refraction_ray;
                                                              refraction_ray_normal.normalize();
                                                              SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     
                                                              this->rt(refraction_ray_normal, point_on_object + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, recursion_depth + 1, shadow_on, reflection_on, refraction_on, RAY_INSIDE);
                                                              //float test0 = 0;
                                                              //float test1 = 0;
                                                              //float test2 = 0;
                                                              //point_on_object.getValue(test0, test1, test2);
                                                              //cout<<"POINT_ON_OBJECT"<<"("<<test0<<","<<test1<<","<<test2<<")"<<endl;
                                                              //SbVec3f test_point = point_on_object+EPSLON*refraction_ray_normal;
                                                              //test_point.getValue(test0, test1, test2);
                                                              //cout<<"POINT_REFRACTION_POINT"<<"("<<test0<<","<<test1<<","<<test2<<")"<<endl;



                                                              refraction_color->getValue(refraction_color0, refraction_color1, refraction_color2);
                                                              cosine_value = refraction_ray_normal.dot(normal);
                                                              color0 = color0 + transparency_factor * refraction_color0;
                                                              color1 = color1 + transparency_factor * refraction_color1;
                                                              color2 = color2 + transparency_factor * refraction_color2;

                                                      }
                                                      else 
                                                      {

                                                              SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     
                                                              //refraction_color0 = 0;
                                                              //refraction_color1 = 0;
                                                              //refraction_color2 = 0;
                                                              ONLY_REFLECTION = True;
                                                              color0 = color0 + transparency_factor * refraction_color0;
                                                              color1 = color1 + transparency_factor * refraction_color1;
                                                              color2 = color2 + transparency_factor * refraction_color2;

                                                      }//only reflection
                                              }//refraction outside
                                        }//end refraction
                              }
                      }
              }
              


                (*color).setValue(color0, color1, color2);

}

/* 
* Name: is_in_shadow
* Argument: 
*          intersect_point: the ray intersect with the sphere point       
*          light_vector: the light vector
*          light_location: the light source location
*          scene: the scene data
*          transform_list: the scene tansform data
* Return value: 1 means in the shadow, -1 means not in the shadow
* Usage:
*        check the intersect point whether in shadow or not. If in shadow return 1, or not in shadow return -1
*/



int MyRayTracer::is_in_shadow(SbVec3f intersect_point, SbVec3f light_vector, SbVec3f light_location, OSUInventorScene *scene, SbMatrix *transform_list)
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
        light_vector.normalize();
        light_vector.negate();
        //float EPSLON = 1e-3;
        //SbVec3f P = intersect_point + EPSLON * light_vector;
        SbVec3f P = intersect_point;
        SbVec3f Ray = light_location - P;

        Ray.normalize();
        SbVec3f intersect_normal(0, 0, 0);
        float distance_length = FAR;


        int is_intersect = -1;
	int transparency_factor = 1;

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
                        SbVec3f normal = P - center_new;
                        normal.normalize();
                        float angle = normal.dot(Ray);
                        is_intersect = this->sphere_intersect(Ray, P, *sphere, point_intersect);
                        if(is_intersect == 1) 
                        {
				if(object->material->transparency[0] <= 0)
				{
					in_shadow = 1;
					transparency_factor *= 0;
					//break;
				}
				else
					transparency_factor *= object->material->transparency[0];
                        }
                }
                else if(shape_type == SoCube::getClassTypeId()) 
                {
                        SoCube *cube = new SoCube();                        
                        SbVec3f cube_point_intersect = *point_intersect;
                        distance_length = this->cube_intersect(Ray, P, cube, transform_list[i], cube_point_intersect, intersect_normal);
                        if(distance_length != FAR) 
                        {
				//if(object->material->transparency[0] == 0||object->material->transparency[0] != NULL) 
				if(object->material->transparency[0] <= 0) 
				{
					in_shadow = 1;
					transparency_factor *= 0;
					//break;
				}
				else
					transparency_factor *= object->material->transparency[0];
                        }
                }
        }
	if(transparency_factor == 0)
	{
		in_shadow = 1;
	}
	else
		in_shadow = -1;

        return in_shadow;

}

int MyRayTracer::refract(SbVec3f d, SbVec3f n, float Eta, SbVec3f *t)
{
        SbVec3f d_neg = d;
        d_neg.negate();
        float N_dot_V = n.dot(d_neg);
	//        float N_dot_V = n.dot(d);
        float under_sqrt = 1 - Eta*Eta*(1 - N_dot_V*N_dot_V);
        if(under_sqrt >= 0)
        {
                //*t = (Eta * N_dot_V - sqrt(under_sqrt))* n - Eta * d_neg;
                *t = (Eta * N_dot_V - sqrtf(under_sqrt))* n - Eta * d_neg;
                return True;

        }
        else
        return False;
}






float MyRayTracer::cube_intersect(SbVec3f ray, SbVec3f eye, SoCube *cube, SbMatrix transform_matrix, SbVec3f &point, SbVec3f &inter_normal)
{
        int is_intersect = True;
        float t_near = -FAR;
        float t_far = FAR;
        SbVec3f object_eye(0, 0, 0);
        SbVec3f object_ray(0, 0, 0);
        SbVec3f object_point(0, 0, 0);
        SbVec3f object_inter_normal(0, 0, 0);
        SbMatrix inverse_matrix = transform_matrix.inverse();
        inverse_matrix.multVecMatrix(eye, object_eye); // Transform Eye coordinate
        inverse_matrix.multDirMatrix(ray, object_ray); // Transform Ray direction

        object_ray.normalize();

        float width = cube->width.getValue();
        float height = cube->height.getValue();
        float depth = cube->height.getValue();
        SbVec3f low(0, 0, 0);
        SbVec3f high(0, 0, 0);

        //low.setValue((-1)*width/2, (-1)*height/2, (-1)*depth/2);
        //high.setValue(width/2, height/2, depth/2);
	low[0] = (-1)*width/2;
	low[1] = (-1)*height/2;
	low[2] = (-1)*depth/2;
	high[0] = width/2;
	high[1] = height/2;
	high[2] = depth/2;
	
        float t1;
        float t2;

        float tmp;
        int i = 0;
        float distance = FAR;
        for(i = 0; i < 3; i++)
        {
                if(fabs(object_ray[i]) <= ZERO)
                {
                        if(object_eye[i] < low[i] || object_eye[i] > high[i])
                        {
                                is_intersect = False;
                                continue;
                        }
                }
                else
                {
                        t1 = (low[i] - object_eye[i])/object_ray[i];
                        t2 = (high[i] - object_eye[i])/object_ray[i];

                        if(t1 > t2)
                        {
                                tmp = t1;
                                t1 = t2;
                                t2 = tmp;
                        }
                        if(t1 > t_near)
                        {
                                t_near = t1;
                        }
                        if(t2 < t_far)
                        {
                                t_far = t2;
                        }
                        if(t_near > t_far) // miss
                        {
                                is_intersect = False;
                                continue;
                        }
                        if(t_far < ZERO) //box behind
                        {
                                is_intersect = False;
                                continue;
                        }

                }
        }
        if(is_intersect == True)
        {
                if(t_near < -ZERO) // if eye is in box
                {
                        object_point = object_eye + t_far * object_ray;
                }
                else
                {
                        object_point = object_eye + t_near * object_ray;
                }

                if(fabs(object_point[0] - low[0]) < ZERO)
                {
                        object_inter_normal.setValue(-1, 0, 0);
                }
                else if(fabs(object_point[0] - high[0]) < ZERO)
                {
                        object_inter_normal.setValue(1, 0, 0);
                }
                else if(fabs(object_point[1] - low[1]) < ZERO)
                {
                        object_inter_normal.setValue(0, -1, 0);
                }
                else if(fabs(object_point[1] - high[1]) < ZERO)
                {
                        object_inter_normal.setValue(0, 1, 0);
                }
                else if(fabs(object_point[2] - low[2]) < ZERO)
                {
                        object_inter_normal.setValue(0, 0, -1);                
                }
                else if(fabs(object_point[2] - high[2]) < ZERO)
                {
                        object_inter_normal.setValue(0, 0, 1);
                }

                transform_matrix.multVecMatrix(object_point, point);
                transform_matrix.multDirMatrix(object_inter_normal, inter_normal);

                inter_normal.normalize();
                SbVec3f distance_vec = eye - point;
                distance = distance_vec.length();           
                if(distance < -FAR)
                distance = FAR;
        }
        else
        {
                distance = FAR;
        }
        return distance;
}
