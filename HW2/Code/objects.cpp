#include "objects.h"

material::~material()
{
	
}

sphere::~sphere(){
    if (mat != NULL)
        delete mat;
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

material* sphere::intersect(){

}