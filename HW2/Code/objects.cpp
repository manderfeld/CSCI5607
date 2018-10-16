#include "objects.h"
#include <cstdio>

#define DEBUG

material::~material()
{
	
}

Sphere::~Sphere(){
    if (mat != NULL)
        delete mat;
    if (next != NULL)
        delete next;
}

void Sphere::add(float x, float y, float z, float r, material* mat){
    if (next == NULL){
        next = new Sphere(x, y, z, r, mat);
    }
    else{
        next->add(x, y, z, r, mat);
    }
    return;
}

intersect* Sphere::hit(Ray* ray){
    #ifdef DEBUG
        //printf("In hit\n");
    #endif
    Vec3 L = O - ray->o;
    Vec3* V = ray->d.UnitVector();
    float tca = dotProd(L, *V); // L * V;

    if(tca < 0) // Exit condition 1
    {
        if(next == NULL)
        {
            return NULL;
        }
        else
        {
            next->hit(ray);
        }
    }

    float d2 = dotProd(L, L) - (tca * tca);

    if (d2 >  r * r)    // Exit condition 2
    {
// QUESTION:
//  Do we need to go to the next object?
//	If d2 > r*r and we don't return, then we'll
//	get a negative value in the squareroot (calculating thc)
    	
        if(next == NULL)
            return NULL;
        else
            return next->hit(ray);
        
        // return NULL;
    }
    float thc = sqrt((r * r) - d2);
    float t_close = tca - thc;
    float t_far = tca + thc;

    #ifdef DEBUG
        //printf("tc %f, tf %f\n", t_close, t_far);
    #endif

    Vec3* temp = ray->d.UnitVector();
    Vec3 P = ray->o + (t_close * *temp);
    delete temp;

    intersect* hit = new intersect(P.x, P.y, P.z, this);
    // printf("Hit Material: %f, %f, %f\n", this->mat->ar, this->mat->ag, this->mat->ab);
    // printf("object %a\n", this);
    return hit;
}