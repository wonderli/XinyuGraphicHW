#include "MyRayTracer.h"

MyRayTracer::MyRayTracer(OSUInventorScene *scene) {
	this->light = NULL;
	this->light_color.setValue(0, 0, 0);
	this->ambient_color.setValue(0, 0, 0);
	this->diffuse_color.setValue(0, 0, 0);
	this->specular_color.setValue(0, 0, 0);
	this->light_intensity = 1;	
	int i = 0;
	static int permutation[] = 
	{
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};


	for (i=0; i < 256 ; i++)
	{
		p[256+i] = this->p[i] = permutation[i];
	}


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

float MyRayTracer::sphere_intersect(SbVec3f ray, SbVec3f eye, SoSphere *sphere, SbMatrix transform_matrix, SbVec3f &point_intersect, SbVec3f &inter_normal) {

	float r = sphere->radius.getValue();
//    cout<<"RADIUS"<<r<<endl;
	//SbVec3f sphere_center = sphere.getCenter();
	SbVec3f sphere_center(0, 0, 0);
	float a, b, c, discriminant;
	float root_1, root_2;
	float root;
	float distance_length = FAR;
	int is_intersect = FALSE;

//	SbVec3f object_eye(0, 0, 0);
//	SbVec3f object_ray(0, 0, 0);
//	SbVec3f object_point(0, 0, 0);
//	SbVec3f object_inter_normal(0, 0, 0);
	SbVec3f object_eye;
	SbVec3f object_ray;
	SbVec3f object_point;
	SbVec3f object_inter_normal;

	SbMatrix inverse_matrix = transform_matrix.inverse();

	inverse_matrix.multVecMatrix(eye, object_eye); // Transform Eye coordinate
	inverse_matrix.multDirMatrix(ray, object_ray); // Transform Ray direction
	object_ray.normalize();



//	SbVec3f d = object_ray;
//	SbVec3f eye_minus_center = object_eye - sphere_center;
//
//	
//	a = d.dot(d);
//	b = 2 * d.dot(object_eye - sphere_center);
//	c = eye_minus_center.dot(eye_minus_center) - r * r;
    a = 1;
    b = 2 * object_eye.dot(object_ray);
    c = object_eye.dot(object_eye) - r * r; 

	discriminant = b * b - 4 * a * c;
	if(discriminant > ZERO) {
		root_1 = (-b - sqrt(discriminant))/(2*a);
		root_2 = (-b + sqrt(discriminant))/(2*a);
		if((root_1 > ZERO) && (root_2 > ZERO)) {
			if (root_1 < root_2) {
				root = root_1;
				is_intersect = TRUE;
			//	*point_intersect = eye + ray * root;
				object_point = object_eye + object_ray * root;
				object_inter_normal = object_point - sphere_center;
			} else {
				root = root_2;
				//is_intersect = 1;
				is_intersect = TRUE;
				//*point_intersect = eye + ray * root;
				object_point = object_eye + object_ray * root;
				object_inter_normal = object_point - sphere_center;
			} 

		} else {
			//is_intersect = -1;
			is_intersect = FALSE;
		}
	}
	if(is_intersect == TRUE)
	{
		transform_matrix.multVecMatrix(object_point, point_intersect);
		inverse_matrix.transpose().multDirMatrix(object_inter_normal, inter_normal);
		inter_normal.normalize(); 
		distance_length = (eye - point_intersect).length(); 
	}
	else
		distance_length = FAR;

	return distance_length;
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

void MyRayTracer::rt(SbVec3f ray, SbVec3f eye, OSUInventorScene *scene, SbMatrix *transform_list, SbVec3f *color, int reflection_depth, int refraction_depth, int shadow_on, int reflection_on, int refraction_on, int ray_location)
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
	float specular_color_transparency0 = 0;			
	float specular_color_transparency1 = 0;			
	float specular_color_transparency2 = 0;			

	SbVec3f *refraction_ray = new SbVec3f(0, 0, 0);
	float light_transparency_factor = 0;
	SbVec3f reflection_ray_normal;
	for (i = 0; i < length; i++)
	{
		object = (OSUObjectData *)scene->Objects[i];
		shape_type = object->shape->getTypeId();
		if(shape_type == SoSphere::getClassTypeId()) 
		{
			SoSphere *sphere = (SoSphere*)(object->shape);
			distance_length = this->sphere_intersect(ray, eye, sphere, transform_list[i], point_on_object, intersect_normal);
			*point_intersect = point_on_object; 
			//intersect_normal.normalize();

		}//end of sphere intersect  
		else if(shape_type == SoCube::getClassTypeId()) 
		{
			SoCube *cube = (SoCube*)(object->shape);
			distance_length = this->cube_intersect(ray, eye, cube, transform_list[i], point_on_object, intersect_normal);
			*point_intersect = point_on_object; 
			//intersect_normal.normalize();

		}//end of cube intersect

		else if(shape_type == SoCylinder::getClassTypeId()) 
		{
			SoCylinder *cylinder = (SoCylinder*)(object->shape);
			distance_length = this->quadric_intersect(ray, eye, cylinder, transform_list[i], point_on_object, intersect_normal);
			*point_intersect = point_on_object; 
			//intersect_normal.normalize();

		}//end of cylinder intersect



		if(distance_length < distance_length_min) 
		{
			closest_object = object;
			distance_length_min = distance_length;
			point_on_object = *point_intersect;
			normal = intersect_normal;
			min_index = i;
		}


	}
	if(distance_length_min < FAR)
	{
		float t1 = ZERO;
		float t2;
		SbVec3f u;
		SbVec3f v;
		SbVec3f w;

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

				t2 = light_vector.length();

				light_vector.normalize();
				SbVec3f reflect_light_vector = light_vector;
				light_vector.negate();
				int hits = 0;
				int light_area = 2;
				/*For softshadow*/
				if(shadow_on == 2)
				{
					this->coordinate_gen(light_vector, u, v, w);
					int m, n;
					SbVec3f start = light_location - light_area/2 * u - light_area/2 * v;
					for(m = 0; m < light_area; m++)
					{
						light_location = start;
						for(n = 0; n < light_area; n++)
						{
							float du = rand()/float(RAND_MAX + 1);
							float dv = rand()/float(RAND_MAX + 1);
							//cout<<"DU"<<du<<"DV"<<dv<<endl;
							light_location = light_location + (du+m)*light_area/10 * u + (dv+n)*light_area/10 * v;
							light_vector = light_location - point_on_object;
							t2 = light_vector.length();
							light_vector.normalize();
							light_transparency_factor = this->object_in_path(point_on_object, light_vector, light_location, transform_list, scene, t1, t2);
							if(light_transparency_factor > ZERO)
							{
								hits++;
							}
						}
						start = start + v;
					}
					light_intensity = light_intensity * hits/(light_area * light_area);
				}


				float N_dot_L = normal.dot(light_vector);
				SbVec3f V = ray;
				V.normalize();
				V.negate();
				float V_dot_N = V.dot(normal);
				R = 2 * N_dot_L * normal - light_vector;
				R.normalize();
				float V_dot_R = V.dot(R);
				object = closest_object;
				shininess_factor = object->material->shininess[0];

				transparency_factor = object->material->transparency[0];//Transparency


				/*ambient color*/
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

				if(shadow_on == 1||shadow_on == 2)
				{
					/* color set to zero for back face light*/
					if (N_dot_L < 0)
					{
						ambient_color.setValue(0, 0, 0);
						diffuse_color.setValue(0, 0, 0);
						specular_color.setValue(0, 0, 0);

					}
					else 
					{

						light_transparency_factor = this->object_in_path(point_on_object, light_vector, light_location, transform_list, scene, t1, t2);
						light_intensity *= light_transparency_factor;
						if(light_transparency_factor <= ZERO)
						{
							ambient_color.setValue(0, 0, 0);
							diffuse_color.setValue(0, 0, 0);
							specular_color.setValue(0, 0, 0);
						}
						else
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
					if (N_dot_L < ZERO)
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
				if(refraction_on == True)
				{
					if(transparency_factor > ZERO)
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
		//Solid Texture, checker board
		SbVec3f cube_point(0, 0, 0);
		float check_board_texture0 = 1;
		float check_board_texture1 = 1;
		float check_board_texture2 = 1;
		transform_list[min_index].inverse().multDirMatrix(point_on_object, cube_point);
		if(closest_object->shape->getTypeId() == SoCube::getClassTypeId())
		{
			if((closest_object->transformation->scaleFactor.getValue())[0]>3 )
			{
				this->checker_board(cube_point[0], cube_point[1], cube_point[2], 0.05, &check_board_texture0, &check_board_texture1, &check_board_texture2);
			}
		}
		color0 *= check_board_texture0;
		color1 *= check_board_texture1;
		color2 *= check_board_texture2;

		//Solid texture, ring, wood grain
		SbVec3f sphere_texture_point(0, 0, 0);
		float ring_texture0 = 1;
		float ring_texture1 = 1;
		float ring_texture2 = 1;
		transform_list[min_index].inverse().multDirMatrix(point_on_object, sphere_texture_point);
		if(closest_object->shape->getTypeId() == SoSphere::getClassTypeId())
		{
			//if((closest_object->transformation->scaleFactor.getValue())[0]>3 )
			{
				//this->rings(sphere_texture_point[0], sphere_texture_point[1], sphere_texture_point[2], 0.05, &ring_texture0, &ring_texture1, &ring_texture2);
				this->wood_grain(sphere_texture_point[0], sphere_texture_point[1], sphere_texture_point[2], 0.05, &ring_texture0, &ring_texture1, &ring_texture2);
			}
		}
		color0 *= ring_texture0;
		color1 *= ring_texture1;
		color2 *= ring_texture2;

		SbVec3f perlin_noise(0, 0, 0);
		SbVec3f perlin_noise_point(0,0,0);
		




	}
	else
	{
		color0 = 0.1;
		color1 = 0.1;
		color2 = 0.1;
	}                


	/* Reflection */
	if(reflection_on == 1)
	{
		if(reflection_depth < MAXRECURSION)
		{
			//if(reflection_on == 1)
			//{
			if(shininess_factor > 0) 
			{       
				reflection_ray = (-2) * ray.dot(normal) * normal + ray;
				reflection_ray_normal = reflection_ray;
				reflection_ray_normal.normalize();

				SbVec3f *reflection_color = new SbVec3f(0, 0, 0);                                      
				this->rt(reflection_ray_normal, point_on_object + EPSLON * reflection_ray_normal, scene, transform_list, reflection_color, reflection_depth + 1,refraction_depth, shadow_on, reflection_on, refraction_on, ray_location);

				reflection_color->getValue(reflection_color0, reflection_color1, reflection_color2);
			}
			if(refraction_on == True)
			{
				if(transparency_factor > ZERO)
				{
					color0 = (color0 + shininess_factor * reflection_color0)*(1 - transparency_factor) + specular_color_transparency0;
					color1 = (color1 + shininess_factor * reflection_color1)*(1- transparency_factor) + specular_color_transparency1;
					color2 = (color2 + shininess_factor * reflection_color2)*(1- transparency_factor) + specular_color_transparency2;

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
			//}
		}
	}
	if(refraction_on == True)
	{
		if(refraction_depth < MAXRECURSION && transparency_factor > ZERO)
		{
			//if(refraction_on == True)
			//{
			if(ray.dot(normal) < 0)
			{
				this->refract(ray, normal, Eta, refraction_ray);
				SbVec3f refraction_ray_normal = *refraction_ray;
				refraction_ray_normal.normalize();
				SbVec3f *refraction_color = new SbVec3f(0, 0, 0); 
				this->rt(refraction_ray_normal, point_on_object + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, reflection_depth, refraction_depth + 1, shadow_on, reflection_on, refraction_on, 1 - ray_location);
				//this->rt(refraction_ray_normal, point_on_object + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, reflection_depth, refraction_depth + 1, shadow_on, reflection_on, refraction_on, RAY_INSIDE);
				refraction_color->getValue(refraction_color0, refraction_color1, refraction_color2);
				color0 = color0 + transparency_factor * refraction_color0;
				color1 = color1 + transparency_factor * refraction_color1;
				color2 = color2 + transparency_factor * refraction_color2;
			}
			else
			{
				SbVec3f normal_neg = normal;
				normal_neg.negate();
				//if(True == this->refract(ray, normal, 1/Eta, refraction_ray))
				if(True == this->refract(ray, normal_neg, 1/Eta, refraction_ray))
				{
					SbVec3f refraction_ray_normal = *refraction_ray;
					refraction_ray_normal.normalize();
					SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     
					this->rt(refraction_ray_normal, point_on_object + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, reflection_depth, refraction_depth + 1, shadow_on, reflection_on, refraction_on, 1 - ray_location);
					//this->rt(refraction_ray_normal, point_on_object + EPSLON * refraction_ray_normal, scene, transform_list, refraction_color, reflection_depth, refraction_depth + 1, shadow_on, reflection_on, refraction_on, RAY_OUTSIDE);

					refraction_color->getValue(refraction_color0, refraction_color1, refraction_color2);
					color0 = color0 + transparency_factor * refraction_color0;
					color1 = color1 + transparency_factor * refraction_color1;
					color2 = color2 + transparency_factor * refraction_color2;

				}
				else 
				{

					SbVec3f *refraction_color = new SbVec3f(0, 0, 0);                                     
					ONLY_REFLECTION = True;
					color0 = color0 + transparency_factor * refraction_color0;
					color1 = color1 + transparency_factor * refraction_color1;
					color2 = color2 + transparency_factor * refraction_color2;

				}//only reflection
			}//refraction outside
			//}//end refraction
		}
	}
	//}



(*color).setValue(color0, color1, color2);

}


int MyRayTracer::refract(SbVec3f d, SbVec3f n, float Eta, SbVec3f *t)
{
	SbVec3f d_neg = d;
	d_neg.negate();
	float N_dot_V = n.dot(d_neg);
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


/* 
 * Name: cube_intersect
 * Argument: 
 *          ray: ray vector shooting from eye to the center of each pixel
 *          eye: eye position
 *          cube: cube need to be tested
 *          point_intersect: the intersection point coordinate.
 * Return value: the distance between camera and intersect point
 * Usage:
 *       Check the ray and cube whether have intersection or not, if have, store the intersection point.
 */





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

	low.setValue((-1)*width/2, (-1)*height/2, (-1)*depth/2);
	high.setValue(width/2, height/2, depth/2);

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
	}
	else
	{
		distance = FAR;
	}
	return distance;
}


/* 
 * Name: object_in_path 
 * Argument: 
 *          intersect_point: the ray intersect with the sphere point       
 *          light_vector: the light vector
 *          light_location: the light source location
 *          transform_list: the scene tansform data
 *          scene: the scene data
 *          t1: ZERO
 *          t2: The distance to light source
 * Return value: transparency factor 
 * Usage:
 *        check the intersect point whether in shadow or not. If in shadow return 1, else it will return the transparency factor about the light
 */

float MyRayTracer::object_in_path(SbVec3f intersect_point, SbVec3f light_vector, SbVec3f light_location, SbMatrix *transform_list, OSUInventorScene *scene,float t1, float t2)
{	
	int i;
	SoType	shapeType;
	OSUObjectData *object = NULL;
	SbVec3f distance;
	float transparency = 1;
	SbVec3f	Point;
	SbVec3f normal;	
	SbVec3f scale_vector;
	float radius;
	SbVec3f center(0,0,0);
	SbVec3f center_new(0,0,0);
	SbVec3f P = intersect_point;
	SbVec3f Ray = light_location - P;
	int is_intersect = -1;
	float distance_length = FAR;
	SbVec3f *point_intersect = &intersect_point;
	SbVec3f intersect_normal(0,0,0);

	for (i = 0; i < scene->Objects.getLength(); i++) 
	{
		object = (OSUObjectData *)scene->Objects[i];
		shapeType = object->shape->getTypeId();


		if(shapeType == SoSphere::getClassTypeId())
		{			
			SoSphere *sphere = (SoSphere*)(object->shape);
			distance_length = this->sphere_intersect(Ray, P, sphere, transform_list[i], intersect_point, normal);

		}
		else if(shapeType == SoCube::getClassTypeId())
		{
			SoCube *cube = (SoCube*)(object->shape);
			distance_length = cube_intersect(Ray, P, cube, transform_list[i], intersect_point, normal);
		}
		else if(shapeType == SoCylinder::getClassTypeId())//CYLINDER
		{
			SoCylinder *cylinder = (SoCylinder*)(object->shape);
			distance_length = quadric_intersect(Ray, P, cylinder, transform_list[i], intersect_point, normal);
		}




		if (distance_length > t1 && distance_length < t2)
		{
			transparency *= object->material->transparency[0];
		}

	}
	return transparency;

}


/* 
 * Name: coordinate_gen
 * Argument: 
 *          light_vector: input vector
 *          u, v, w: the direction of vector of input vector
 * Return value: void 
 * Usage:
 *       generate the direction vector u, v, w of input vector
 */
void MyRayTracer::coordinate_gen(SbVec3f light_vector, SbVec3f &u, SbVec3f &v, SbVec3f &w)
{
	light_vector.normalize();
	w = light_vector;
	float min;
	SbVec3f vec;
	min = w[0];
	int min_index = 0;
	if(w[1] < min)
	{
		min = w[1];
		min_index = 1;
	}
	if(w[2] < min)
	{
		min = w[2];
		min_index = 2;
	}

	if(min_index == 0)
	{
		vec.setValue(1, w[1], w[2]);
	}else if(min_index == 1)
	{
		vec.setValue(w[0], 1, w[2]);
	}else if(min_index == 2)
	{
		vec.setValue(w[0], w[1], 1);
	}
	u = vec.cross(w);
	u.normalize();
	v = w.cross(u);
	v.normalize();

}
/* 
 * Name: checkerboard 
 * Argument: 
 * 			x, y, z, the cooradinate of point
 * 			size, the size of checker board per tile
 * 			color0, color1, color2, the color generated
 * Return value: void 
 * Usage:
 * 			Draw the checker borad.
 */

void MyRayTracer::checker_board(float x, float y, float z, float size, float *color0, float *color1, float *color2)
{
	float A = 100;
	int jump = ((int)(A + x/size) + (int)(A + y/size) + (int)(A + z/size))%2;
	if(jump == 0)
	{
		*color0 = 0.5;
		*color1 = 0.5;
		*color2 = 0.5;

	}
	else if(jump == 1)
	{
		*color0 = 0.8;
		*color1 = 0.8;
		*color2 = 0.8;
	}
}
/* 
 * Name: rings 
 * Argument: 
 * 			x, y, z, the cooradinate of point
 * 			size, the size of checker board per tile
 * 			color0, color1, color2, the color generated
 * Return value: void 
 * Usage:
 * 			Draw the ring texture.
 */

void MyRayTracer::rings(float x, float y, float z, float size, float *color0, float *color1, float *color2)
{
	float m = 0.2;
	int r = (int)(sqrt(x * x+y * y+ z * z)/m)%2;
	if(r == 0)
	{
		*color0 = 0.9;
		*color1 = 0.0;
		*color2 = 0.2;
	}
	else if(r == 1)
	{
		*color0 = 0.0;
		*color1 = 0.2;
		*color2 = 0.8;
	}

}
/* 
 * Name: wood_grain 
 * Argument: 
 * 			x, y, z, the cooradinate of point
 * 			size, the size of checker board per tile
 * 			color0, color1, color2, the color generated
 * Return value: void 
 * Usage:
 * 			Draw the wood grain texture.
 */

void MyRayTracer::wood_grain(float x, float y, float z, float size, float *color0, float *color1, float *color2)
{

	float M = 0.03;
	float N = 2;
	float theta = 0;
	float k = 2;
	float r = 0;
	float rings = 0;
	float bz = 0.1;
	//SbVec3f A(0.2, 0.2, 0.2);
	//SbVec3f A(1, 1, 1);
	SbVec3f A(0.4, 0.4, 0.4);
	SbVec3f D(0.5, 0.3, 0.3);
	SbVec3f v;
	v.setValue(x, y, z);
	v.normalize();
	theta = acos(SbVec3f(0, 0, 1).dot(v));
	r = sqrt(x * x + y * y + z * z);
	rings = ((int)(r/M + k * sin(theta/N + 0.1)))%2;
	SbVec3f tmp(0, 0, 0);
	//tmp = D + A * rings;
	//tmp = D + A * rings;
	tmp = D + A * rings;
	tmp.getValue(*color0, *color1, *color2);
}

/* 
 * Name: quadric_intersect
 * Argument: 
 *          ray: ray vector shooting from eye to the center of each pixel
 *          eye: eye position
 *          cylinder: cylinder need to be tested
 *          point_intersect: the intersection point coordinate.
 * Return value: the distance between camera and intersect point
 * Usage:
 *       Check the ray and cylinder whether have intersection or not, if have, store the intersection point.
 */


float MyRayTracer::quadric_intersect(SbVec3f ray, SbVec3f eye, SoCylinder *cylinder, SbMatrix transform_matrix, SbVec3f &point_intersect, SbVec3f &inter_normal) {

	//float radius = cylinder->radius.getValue();
	//float height = cylinder->height.getValue();
	float a, b, c, discriminant;
	float root_1, root_2;
	float root;
	float distance_length = FAR;
	int is_intersect = FALSE;
	float A, B, C, D, E, F, G, H, I, J;





	SbVec3f object_eye;
	SbVec3f object_ray;
	SbVec3f object_point;
	SbVec3f object_inter_normal;

	SbMatrix inverse_matrix = transform_matrix.inverse();

	inverse_matrix.multVecMatrix(eye, object_eye); // Transform Eye coordinate
	inverse_matrix.multDirMatrix(ray, object_ray); // Transform Ray direction
	object_ray.normalize();



	//SbVec3f D = object_ray;
	float xd = object_ray[0];
	float yd = object_ray[1];
	float zd = object_ray[2];

	float xr = object_eye[0];
	float yr = object_eye[1];
	float zr = object_eye[2];
//	A = 100, B = -25, C = 4, D = E = F = 0, G = -200, H = -100, I = -8, J = 104; 
	A = 1, B = 0, C = 1, D = E = F = 0, G = 0, H = 0, I = 0 , J = -0.1;//J = -1; 
	
	a = A * xd * xd + B * yd * yd + C * zd * zd + D * xd * yd + E * xd * zd + F * yd * zd;
	b = 2 * A * xr * xd + 2 * B * yr * yd + 2 * C * zr * zd + D * (xr * yd + yr * xd) + E * xr * zd + F * (yr * zd + yd * zr) + G * xd + H * yd + I * zd;
	c = A * xr * xr + B * yr * yr + C * zr * zr + D * xr * yr + E * xr * zr + F * yr * zr + G * xr + H * yr + I * zr + J;
	
	float low = 0;
	float high = low + 11;

	SbVec3f P0 = object_eye;
	SbVec3f P1_low(0, low, 0);
	SbVec3f P1_high(0, high, 0);
	SbVec3f u = object_ray;
	SbVec3f n_low(0, -1, 0);
	SbVec3f n_high(0, 1, 0);
	float t_low = 0;
	float t_high = 0;
	int hit_flag = 0;
	float min = 999;

	discriminant = b * b - 4 * a * c;
	if(discriminant > ZERO) {
		root_1 = (-b - sqrt(discriminant))/(2*a);
		root_2 = (-b + sqrt(discriminant))/(2*a);
		if((root_1 > ZERO) && (root_2 > ZERO)) {
			if (root_1 < root_2) {
				root = root_1;
				min = root;
				object_point = object_eye + object_ray * root;
				object_inter_normal.setValue(object_point[0], 0, object_point[2]);
				if((object_point[1] > low) && (object_point[1] < high))
				{
					hit_flag++;
				}

				
			} else {
				root = root_2;
				min = root;
				object_point = object_eye + object_ray * root;
				object_inter_normal.setValue(object_point[0], 0, object_point[2]);
				if((object_point[1] > low) && (object_point[1] < high))
				{
					hit_flag++;
				}

			} 
		} 
	}

	if((n_low.dot(u) > ZERO) || (n_low.dot(u) < -ZERO ))
	{
		t_low = (n_low.dot(P1_low) - n_low.dot(P0))/n_low.dot(u);
		if(t_low > ZERO)
		{
			object_point = object_eye + object_ray * t_low;
			if((object_point[0] * object_point[0] + object_point[2] * object_point[2]) < (-J) )
			{
				if(t_low < min)
				{
					min = t_low;
					object_inter_normal.setValue(0, -1, 0);
					hit_flag++;
				}
			}
		}
	}
	if((n_high.dot(u) > ZERO) || (n_high.dot(u) < -ZERO ))
	{
		t_high = (n_high.dot(P1_high) - n_high.dot(P0))/n_high.dot(u);
		if(t_high > ZERO)
		{
			object_point = object_eye + object_ray * t_high;
			if((object_point[0] * object_point[0] + object_point[2] * object_point[2] )< (-J) )
			{
				if(t_high < min)
				{
					min = t_high;
					object_inter_normal.setValue(0, 1, 0);
					hit_flag++;
				}
			}
		}
	}
	if(hit_flag > 0)
	{
		is_intersect = TRUE;
	}
	else
		is_intersect = FALSE;



	if(is_intersect == TRUE)
	{
		transform_matrix.multVecMatrix(object_point, point_intersect);
		inverse_matrix.transpose().multDirMatrix(object_inter_normal, inter_normal);
		inter_normal.normalize(); 
		distance_length = (eye - point_intersect).length(); 
	}
	else
		distance_length = FAR;
	return distance_length;
}

float MyRayTracer::fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10); 
}
float MyRayTracer::lerp(float t, float a, float b) 
{ 
	return a + t * (b - a); 
}

float MyRayTracer::grad(int hash, float x, float y, float z) {
	int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
	double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
		   v = h<4 ? y : h==12||h==14 ? x : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}
float MyRayTracer::noise(float x, float y, float z) {
	int X = (int)floor(x) & 255;                  // FIND UNIT CUBE THAT
	int	Y = (int)floor(y) & 255;                  // CONTAINS POINT.
	int	Z = (int)floor(z) & 255;
	x -= floor(x);                                // FIND RELATIVE X,Y,Z
	y -= floor(y);                                // OF POINT IN CUBE.
	z -= floor(z);
	float u = this->fade(x);                               // COMPUTE FADE CURVES
	float v = this->fade(y);                                // FOR EACH OF X,Y,Z.
	float w = this->fade(z);
	int A = this->p[X]+Y;
	int AA = this->p[A]+Z;
	int AB = this->p[A+1]+Z;      // HASH COORDINATES OF
	int	B = this->p[X+1]+Y;
	int BA = this->p[B]+Z;
	int BB = this->p[B+1]+Z;      // THE 8 CUBE CORNERS,

	return this->lerp(w, this->lerp(v, this->lerp(u, this->grad(p[AA], x, y, z),  // AND ADD
					this->grad(this->p[BA], x-1, y, z)), // BLENDED
				this->lerp(u, this->grad(this->p[AB], x, y-1, z),  // RESULTS
					this->grad(this->p[BB], x-1, y-1, z))),// FROM  8
			this->lerp(v, this->lerp(u, this->grad(this->p[AA+1], x  , y  , z-1 ),  // CORNERS
					this->grad(this->p[BA+1], x-1, y, z-1)), // OF CUBE
				this->lerp(u, this->grad(this->p[AB+1], x, y-1, z-1),
					this->grad(this->p[BB+1], x-1, y-1, z-1))));
}

float MyRayTracer::turb(float scale, double x, double y, double z)
{ 
	float t = 0.0;
	//int scale = 1;
	
	for (int k = 0; k <= scale; k++)
	{
		//t += 1.0/pow(2,(k+1))*noise(pow(2.0,k)*s, x, y, z);
		t += fabs(this->noise(x/scale, y/scale, z/scale)*scale);
		scale /= 0.2;
	}
	
	return t;
}





