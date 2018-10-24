//Objects.h
//
//Class representing various objects within the scene

#include <cmath>
#include <cstddef>
#include "vector.h"
#include <cstdio>

// for testing
#include <iostream>
using namespace std;

#ifndef OBJECTS
#define OBJECTS

struct Light
{
    float r;
    float g;
    float b;
    Vec3 position;

    Light(float r, float g, float b, float x, float y, float z) : r(r), g(g), b(b), position(x, y, z) {}
    ~Light();
};

class Camera
{
public:
    Vec3 O;
    Vec3 D;
    Vec3 U;
    float ha;

    Camera() : O(0, 0, 0), D(0, 0, 1), U(0, 1, 0), ha(45) {}
    Camera(const Vec3* o, const Vec3* d, const Vec3* u, float ha) : O(o->x, o->y, o->z), D(d->x, d->y, d->z), U(u->x, u->y, u->z), ha(ha) {}
    Camera(float ox, float oy, float oz, float dx, float dy, float dz, float ux, float uy, float uz, float ha)
        : O(ox, oy, oz), D(dx, dy, dz), U(ux, uy, uz), ha(ha) {}
};

class material
{
public:
    float ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior;

    material() : ar(0), ag(0), ab(0), dr(1), dg(1), db(1), sr(0), sg(0), sb(0), ns(5), tr(0), tg(0), tb(0), ior(1) {}
    material(float ar, float ag, float ab, float dr, float dg, float db, float sr, float sg, float sb, float ns, float tr, float tg, float tb, float ior)
        : ar(ar), ag(ag), ab(ab), dr(dr), dg(dg), db(db), sr(sr), sg(sg), sb(sb), ns(ns), tr(tr), tg(tg), tb(tb), ior(ior) {}
    ~material();
    material& operator = (const material &a) // don't want to change a's values
    {
        ar = a.ar;
        ag = a.ag;
        ab = a.ab;
        dr = a.dr;
        dg = a.dg;
        db = a.db;
        sr = a.sr;
        sg = a.sg;
        sb = a.sb;
        ns = a.ns;
        tr = a.ns;
        tg = a.tg;
        tb = a.tb;
        ior = a.ior;
        return *this;
    }
};

class intersect;

class Sphere
{
public:
    float x, y, z, r;
    Vec3 O;
    Sphere* next;
    material* mat;

    Sphere() : x(0), y(0), z(0), r(0), next(NULL){}
    Sphere(float x, float y, float z, float r, material* mat) : x(x), y(y), z(z), r(r), next(NULL) 
    {
        O.x = x;
        O.y = y;
        O.z = z;
        this->mat = new material();
        *this->mat = *mat;
        // printf("Material: %f, %f, %f\n", this->mat->ar, this->mat->ag, this->mat->ab);
    }
    ~Sphere();
    void add(float x, float y, float z, float r, material* mat);
    intersect* hit(Ray* ray);
};

class intersect
{
public:
    Sphere* obj;
    Vec3 hit;

    intersect(float x, float y, float z, Sphere* o)
    {
        hit.x = x;
        hit.y = y;
        hit.z = z;
        obj = o;
    }
};

// make a class object that sphere and triange inherit from
// then we can iterate through a vector of objects or something

class Tintersect;
class Triangle
{
public:
    Vec3 v1, v2, v3;
    Vec3 n; // normal vector
    Triangle* next;
    material* mat;

    Triangle() : v1(0.0,0.0,0.0), v2(0.0,0.0,0.0), v3(0.0,0.0,0.0), n(0.0,0.0,0.0), next(NULL) {}
    
    // No normal specified, we have to calculate it
    Triangle(Vec3 v1, Vec3 v2, Vec3 v3, Vec3 n, material *mat) : v1(v1), v2(v2), v3(v3), n(n), next(NULL)
    {
        this->mat = new material();
        *this->mat = *mat;
    }
    ~Triangle();
    void add(Vec3 v1, Vec3 v2, Vec3 v3, Vec3 n, material*mat);
    Tintersect* hit(Ray* ray);
};

class Tintersect
{
public:
    Triangle* obj;
    Vec3 hit;

    Tintersect(float x, float y, float z, Triangle* o)
    {
        hit.x = x;
        hit.y = y;
        hit.z = z;
        obj = o;
    }
};

#endif