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

                void MyRayTracer::rt(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbVec3f *color, int recursion_depth, int shadow_on, int reflection_on, int ray_location)
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
                        int min_index = 0;
                        float shininess_factor = 0;
                        float transparency_factor = 0;
                        SbVec3f reflection_ray(0, 0, 0);
                        SbVec3f *refraction_ray = new SbVec3f(0, 0, 0);
                        float reflection_color0 = 0;
                        float reflection_color1 = 0;
                        float reflection_color2 = 0;
                        float EPSLON = 1e-3;
                        float Eta = 1;
                        float cosine_value = 0;
                        float refraction_color0 = 0;
                        float refraction_color1 = 0;
                        float refraction_color2 = 0;


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
                        if(ray_location == RAY_INSIDE)
                        {
                                normal = normal * (-1);
                                Eta = 1 / Eta;
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
                                        light_vector = point_on_sphere - light_location; 
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
                                        object = (OSUObjectData *)scene->Objects[min_index];
                                        shininess_factor = object->material->shininess[0];
                                        if(object->material->transparency[0] != NULL)
                                        {
                                                transparency_factor = object->material->transparency[0];
                        //                        Eta = 1 / transparency_factor;
                                                Eta = transparency_factor;                        
                                        }

                                        /*ambient color*/
                                        float ambient_color0 = object->material->ambientColor[0][0];
                                        float ambient_color1 = object->material->ambientColor[0][1];
                                        float ambient_color2 = object->material->ambientColor[0][2];
                                        ambient_color.setValue(ambient_color0, ambient_color1, ambient_color2);

                                        if(shadow_on == 1)
                                        {
                                                /* color set to zero for back face light*/
                                                if (N_dot_L < 0)
                                                {
                                                        ambient_color.setValue(0, 0, 0);
                                                        diffuse_color.setValue(0, 0, 0);
                                                        specular_color.setValue(0, 0, 0);

                                                }else 
                                                {
                                                        is_inshadow = is_in_shadow(point_on_sphere, light_vector,  light_location, scene, transform_list);                             
                                                        /* in shadow */
                                                        if(is_inshadow == 1)
                                                        {
                                                                ambient_color.setValue(0, 0, 0);
                                                                diffuse_color.setValue(0, 0, 0);
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

                                                        }
                                                }

                                        }
                                else if(shadow_on == 0)
                                {
                                        if (N_dot_L < 0)
                                        {
                                                ambient_color.setValue(0, 0, 0);
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

                                color0 += 0.2 * ambient_color[0] + N_dot_L * diffuse_color[0] * light_intensity * light_color[0] + pow(V_dot_R, 50) * specular_color[0] * light_intensity * light_color[0];
                                color1 += 0.2 * ambient_color[1] + N_dot_L * diffuse_color[1] * light_intensity * light_color[1] + pow(V_dot_R, 50) *specular_color[1] * light_intensity * light_color[1];
                                color2 += 0.2 * ambient_color[2] + N_dot_L * diffuse_color[2] * light_intensity * light_color[2] + pow(V_dot_R, 50) *specular_color[2] * light_intensity * light_color[2];



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
//                                      ray_location = RAY_OUTSIDE;
                                      this->rt(reflection_ray_normal, point_on_sphere + EPSLON * reflection_ray_normal, scene, transform_list, reflection_color, recursion_depth + 1, shadow_on, reflection_on, ray_location);
                                      float reflection_color0 = 0;
                                      float reflection_color1 = 0;
                                      float reflection_color2 = 0;
                                      reflection_color->getValue(reflection_color0, reflection_color1, reflection_color2);
                                      color0 = color0 + shininess_factor * reflection_color0;
                                      color1 = color1 + shininess_factor * reflection_color1;
                                      color2 = color2 + shininess_factor * reflection_color2;
                              }

                              /* Refraction */
                              if(Eta > 0)
                              {

                                      if(ray.dot(normal) < 0)
                                      {

                                              this->refract(ray, normal, Eta, refraction_ray);
                                              SbVec3f refraction_ray_normal = *refraction_ray;
                                              refraction_ray_normal.normalize();
                                              SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     
                                              ray_location = RAY_OUTSIDE;
                                              this->rt(refraction_ray_normal, point_on_sphere + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, recursion_depth + 1, shadow_on, reflection_on, ray_location);
                                              float refraction_color0 = 0;
                                              float refraction_color1 = 0;
                                              float refraction_color2 = 0;
                                              refraction_color->getValue(refraction_color0, refraction_color1, refraction_color2);
                                              cosine_value = normal.dot(ray*(-1));
                                              //cosine_value = (-1) * normal.dot(ray);
                                              float R_Zero = ((Eta - 1)*(Eta - 1)) / ((Eta + 1) * (Eta +1));
                                              float R_Value = R_Zero + (1 - R_Zero)*pow((1 - cosine_value), 5);
                                              color0 = color0 + R_Value * reflection_color0 + (1 - R_Value) * refraction_color0;
                                              color1 = color1 + R_Value * reflection_color1 + (1 - R_Value) * refraction_color1;
                                              color2 = color2 + R_Value * reflection_color2 + (1 - R_Value) * refraction_color2;


                                      }
                                      else
                                      {
                                              if(True == this->refract(ray, normal*(-1), 1/Eta, refraction_ray))
                                              {
                                                      SbVec3f refraction_ray_normal = *refraction_ray;
                                                      refraction_ray_normal.normalize();
                                                      SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     
                                                      ray_location = RAY_INSIDE;
                                                      this->rt(refraction_ray_normal, point_on_sphere + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, recursion_depth + 1, shadow_on, reflection_on, ray_location);
                                                      refraction_color0 = 0;
                                                      refraction_color1 = 0;
                                                      refraction_color2 = 0;
                                                      refraction_color->getValue(refraction_color0, refraction_color1, refraction_color2);
                                                      cosine_value = refraction_ray_normal.dot(normal);
                                                      float R_Zero = ((Eta - 1)*(Eta - 1)) / ((Eta + 1) * (Eta +1));
                                                      float R_Value = R_Zero + (1 - R_Zero)*pow((1 - cosine_value), 5);
                                                      color0 = color0 + R_Value * reflection_color0 + (1 - R_Value) * refraction_color0;
                                                      color1 = color1 + R_Value * reflection_color1 + (1 - R_Value) * refraction_color1;
                                                      color2 = color2 + R_Value * reflection_color2 + (1 - R_Value) * refraction_color2;

                                              }
                                              else 
                                              {
                                                      SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     
                                                      refraction_color0 = 0;
                                                      refraction_color1 = 0;
                                                      refraction_color2 = 0;
                                                      cosine_value = 0;
                                              }
                                      }

                              }

                    }


              }
//              float R_Zero = ((Eta - 1)*(Eta - 1)) / ((Eta + 1) * (Eta +1));
//              float R_Value = R_Zero + (1 - R_Zero)*pow((1 - cosine_value), 5);
//              color0 = color0 + R_Value * reflection_color0 + (1 - R_Value) * refraction_color0;
//              color1 = color1 + R_Value * reflection_color1 + (1 - R_Value) * refraction_color1;
//              color2 = color2 + R_Value * reflection_color2 + (1 - R_Value) * refraction_color2;

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
        float EPSLON = 1e-3;
        SbVec3f P = intersect_point + EPSLON * light_vector;
        SbVec3f Ray = light_location - P;
        Ray.normalize();


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
                        SbVec3f normal = P - center_new;
                        normal.normalize();
                        float angle = normal.dot(Ray);
                        is_intersect = this->sphere_intersect(Ray, P, *sphere, point_intersect);
                        if(is_intersect == 1) 
                        {
                                in_shadow = 1;
                                break;
                        }
                }
        }

        return in_shadow;

}

int MyRayTracer::refract(SbVec3f d, SbVec3f n, float Eta, SbVec3f *t)
{
        SbVec3f d_neg = (-1) * d;
        float N_dot_V = n.dot(d_neg);
        float under_sqrt = 1 - Eta*Eta*(1 - N_dot_V*N_dot_V);
        if(under_sqrt >= 0)
        {
                *t = (Eta * N_dot_V - sqrt(under_sqrt) )* n + Eta * d;
                return True;

        }
        else
        return False;
}