#ifndef _VISMESH
#define _VISMESH

#include <gmesh.h>
#include <gsim.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MTime.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MTypeId.h>

class visMesh : public MPxNode {
public:
    // Constructor/destroyer
	         visMesh() {};
    virtual ~visMesh() {};

    // MPxNode overrides and setup
    virtual MStatus compute (const MPlug& plug, MDataBlock& data);
    static void*    creator();
    static MStatus  initialize();

    //Attributes
    static MObject   outputMesh;
    static MObject   time;
    //static MObject   steps;
	static MObject   initFile;
public:
    static MTypeId   id;
    static gsim*     simulator;

protected:
    // Adds a tiny cube to timeframe 1. Will most likely never get called.
    static void buildCube();

    // Loops through the mesh store and destroys all the meshes properly.
    static void destroyMeshes();

    // Retrieve the mesh out of the mesh stores or a cube if none is found.
    MObject getMesh(const MTime& time, MObject& outData, MStatus& stat); 

    // Convert all the arguments to a string.
	std::string args2string(MDataBlock& data);

    // Print a MFloatPointArray to cout.
    void printMFPA(MFloatPointArray arr);

    // Print the vertices of all meshes in the stores.
    void printMeshes();
};

#endif //_VISMESH