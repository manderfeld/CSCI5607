#include "objects.h"
#include <cstdio>

#define DEBUG

material::~material()
{
	
}

sphere::~sphere(){
    if (mat != NULL)
        delete mat;
    if (next != NULL)
        delete next;
}

void sphere::add(float x, float y, float z, float r, material* mat){
    if (next == NULL){
        next = new sphere(x, y, z, r, mat);
    }
    else{
        next->add(x, y, z, r, mat);
    }
    return;
}

intersect* sphere::hit(Ray* ray){
    #ifdef DEBUG
        printf("In hit\n");
    #endif
    Vec3 L = O - ray->o;
    Vec3* V = ray->d.UnitVector();
    float tca = dotProd(L, *V); // L * V;
    if(tca < 0) // Exit condition 1
    {
        if(next == NULL)
            return NULL;
        else
            next->hit(ray);
    }

    float d2 = dotProd(L, L) - (tca * tca);
    if (d2 >  r * r)    // Exit condition 2
    {
        if(next == NULL)
            return NULL;
        else
            next->hit(ray);
    }

    float thc = sqrt((r * r) - d2);
    float t_close = tca - thc, t_far = tca + thc;

    #ifdef DEBUG
        printf("tc %f, tf %f\n", t_close, t_far);
    #endif

    Vec3* temp = ray->d.UnitVector();
    Vec3 P = ray->o + (t_close * *temp);
    delete temp;

    intersect* hit = new intersect(P.x, P.y, P.z, this);
    return hit;
}