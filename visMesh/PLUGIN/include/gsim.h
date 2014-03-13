#ifndef _GSIM
#define _GSIM

#include <gmesh.h>

#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MDoubleArray.h>

// All simulator interfaces should implement this virtual class for the plugin to work.

class gsim {
public:
	gsim() {};
    virtual ~gsim() {}

    // Gets a list of names for the arguments the simulator wants to have. All arguments are assumed to be doubles.
    virtual MStringArray* getArguments() = 0;

    // Initialize the simulator with the given double arguments. (IE, the attributes collected from the above arguments)
	// After this call, the simulator should be able to present a mesh!
	virtual void   initialize(MString initFile, MDoubleArray params) = 0;

    // Causes the simulator to progress the simulation with a constant ticksize.
    virtual void tick() = 0;

    // retrieve a pointer to the mesh that currently is active in the simulator.
    //virtual gmesh* getMesh() = 0;

    virtual MFloatPointArray getMeshVerts() = 0;
    virtual MIntArray getMeshFaceCounts() = 0;
    virtual MIntArray getMeshFaceConnects() = 0;

    // What time-step is the simulator at?
    virtual int getTime() = 0;

public:
};

#endif //_GSIM
