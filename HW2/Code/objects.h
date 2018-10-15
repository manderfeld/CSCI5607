//Objects.h
//
//Class representing various objects within the scene

#include <cmath>
#include <cstddef>

// for testing
#include <iostream>
using namespace std;

#ifndef OBJECTS
#define OBJECTS

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

class sphere
{
public:
    float x, y, z, r;
    sphere* next;
    material* mat;

    sphere() : x(0), y(0), z(0), r(0), next(NULL){}
    sphere(float x, float y, float z, float r, material* mat) : x(x), y(y), z(z), r(r), next(NULL) 
    {
        this->mat = mat;
    }
    ~sphere();
    void add(float x, float y, float z, float r, material* mat);
    material* intersect();
};

#endif