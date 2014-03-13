/* This is a sample implementation of a mesh, to test the abstract class */

#ifndef _CUBEMESH
#define _CUBEMESH

#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <gmesh.h>


class cubeMesh : public gmesh {
public:
    cubeMesh();
    virtual ~cubeMesh() {}

public:
	MFloatPointArray vertices;
	MIntArray faceCounts;
	MIntArray faceConnects;
};

#endif //_CUBEMESH
