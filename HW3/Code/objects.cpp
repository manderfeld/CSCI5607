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

Tintersect* Triangle::hit(Ray* ray)
{
    Vec3 rayOrg = ray->o; // P
    Vec3 rayDir = ray->d; // d

    // Exit case 1
    // if ray and plane are parallel then they don't intersect!
    if ( (dotProd(rayDir,n) == 0) || ((dotProd(rayDir,n) >= -0.001)&&(dotProd(rayDir,n) <= 0.001)) ) // if dot product is 0 or almost 0
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

    // ray-plane intersection
    float D = dotProd(n, v1); // plane coefficient, can do dot product with any vertex, just do the first one
//printf("D: %f\n", D);
    float t = (dotProd(n, rayOrg) + D) / dotProd(n, rayDir); 
    // if this doesn't work, try flipping the sign?
    //float t = - (dotProd(n, rayOrg) + D) / dotProd(n, rayDir);

    // Exit case 2
    if (t < 0) // triangle is behind the ray
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

    Vec3 Q = rayOrg + t*rayDir; // plug into parametric equation

    // Now see if the point Q actually lies within the triangle (if it does we can return an intersection object, if not we exit!)
    // a = v1
    // b = v2
    // c = v3
    Vec3 ba = v2 - v1;
    Vec3 ca = v3 - v1;
    Vec3 cb = v3 - v2;
    Vec3 ac = v1 - v3;
    Vec3 qa = Q - v1;
    Vec3 qb = Q - v2;
    Vec3 qc = Q - v3;

    Vec3* one = crossProd(ba,qa);
    Vec3* two = crossProd(cb,qb);
    Vec3* three = crossProd(ac,qc);

    // Exit case 3
    float t1 = dotProd(*one,n);
    float t2 = dotProd(*two,n);
    float t3 = dotProd(*three,n);
    if (t1 < 0)
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
    if (t2 < 0)
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
    if (t3 < 0)
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

    // okay, from here we know that Q is inside the triangle
    
    // Barycentric coordinates
    Vec3* four = crossProd(ba,ca);

    float alpha; // (two dot n) / (four dot n)
    float beta; // (three dot n)
    float gamma; // (one)

    Vec3* five;
    Vec3* six;


delete one; delete two; delete three; delete four;

Tintersect* hit = new Tintersect(Q.x, Q.y, Q.z, this);
return hit;
}