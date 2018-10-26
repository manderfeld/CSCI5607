#include "objects.h"
#include <cstdio>

#define DEBUG

Light::~Light()
{
	
}

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
        // printf("In hit\n");
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
            return next->hit(ray);
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

    if (t_close < t_far) // if t_far is actually closer
    {
        // swap them
        float temp = t_far;
        t_far = t_close;
        t_close = t_far;
        if (t_close < 0) // if less than zero, it's a miss
        {
            if(next == NULL)
            {
                return NULL;
            }
            else
            {
                return next->hit(ray);
            }
        }
    }

    #ifdef DEBUG
        //printf("tc %f, tf %f\n", t_close, t_far);
    #endif

    if (t_close < 0 && t_far < 0){
        if(next == NULL)
            return NULL;
        else
            return next->hit(ray);
    }

    Vec3* temp = ray->d.UnitVector();
    Vec3 P = ray->o + (t_close * *temp);
    delete temp;

    intersect* hit = new intersect(P.x, P.y, P.z, this);
    // printf("Hit Material: %f, %f, %f\n", this->mat->ar, this->mat->ag, this->mat->ab);
    // printf("object %a\n", this);
    return hit;
}

/*************************************************
 *  FOR TRIANGLES!!!!                             *
 *************************************************/
Triangle::~Triangle(){
    if (mat != NULL)
        delete mat;
    if (next != NULL)
        delete next;
}

void Triangle::add(Vec3 v1, Vec3 v2, Vec3 v3, Vec3 n, material* mat)
{
    if (next == NULL){
        next = new Triangle(v1, v2, v3, n, mat);
    }
    else{
        next->add(v1, v2, v3, n, mat);
    }
    return;
}


// Moller-Trumbore intersection algorithm
intersect* Triangle::hit(Ray* ray)
{
//printf("in hit\n");
    Vec3 rayOrg = ray->o; // P
    Vec3 rayDir = ray->d; // d

    Vec3 vert0 = v1;
    Vec3 vert1 = v2;
    Vec3 vert2 = v3;
    Vec3 edge1, edge2, s;
    Vec3* hp;
    Vec3* qp;
    //Vec3 h;
    float a, f, u, v;

    edge1 = vert1 - vert0;
    edge2 = vert2 - vert0;
    hp = crossProd(rayDir, edge2);
    a = dotProd(edge1, *hp);
    if ( a > 0.0000001 && a < 0.0000001) // parallel case
    {
        delete hp;
        if(next == NULL)
        {
            return NULL;
        }
        else
        {
            return next->hit(ray);
        }  
    }
    f = 1.0/a;
    s = rayOrg - vert0;
    u = f * (dotProd(s,*hp));
    if (u < 0.0 || u > 1.0)
    {
        if(next == NULL)
        {
            return NULL;
        }
        else
        {
            return next->hit(ray);
        }
    }
    qp = crossProd(s, edge1);
    v = f * (dotProd(rayDir, *qp));
    delete hp;
    if (v < 0.0 || u + v > 1.0)
    {
        if(next == NULL)
        {
            return NULL;
        }
        else
        {
            return next->hit(ray);
        }
    }
    float t = f * (dotProd(edge2,*qp));
    delete qp;
    if ( t > 0.0000001) // ray intersection
    {
        Vec3 Q = rayOrg + t*rayDir;
        intersect* hit = new intersect(Q.x, Q.y, Q.z, this);
        return hit;
    }
    else
    {
        if(next == NULL)
        {
            return NULL;
        }
        else
        {
            return next->hit(ray);
        }
    }
}