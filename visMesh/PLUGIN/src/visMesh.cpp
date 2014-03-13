#include <macros.h>
#include <visMesh.h>

#include <ChanVese.h>

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>

#include <string>
#include <vector>

std::string lastConfig;
bool debug = true; // Makes the plugin print debug statements if any.
MObjectArray simAttrs;
std::vector<MFloatPointArray> vertices;
std::vector<MIntArray> faceCounts;
std::vector<MIntArray> faceConnects;

MObject   visMesh::time;
MObject   visMesh::initFile;
MObject   visMesh::outputMesh;
gsim*     visMesh::simulator;

MTypeId   visMesh::id(0x08483);
/* Note about ID's:
 * According to http://docs.autodesk.com/MAYAUL/2013/ENU/Maya-API-Documentation/cpp_ref/class_m_type_id.html
 * We can freely use the range 0x00000-0x7ffff without colliding with Maya
 * or any of the tutorial plugins, so we will keep them there.
 */

#pragma region helpers
void visMesh::printMFPA(MFloatPointArray arr) {
    for (unsigned int i = 0 ; i < arr.length() ; i++) {
        MFloatPoint pt = arr[i];
        cout << pt.x << "," << pt.y << "," << pt.z << endl;
    }
}

void visMesh::printMeshes() {
    for (std::vector<MFloatPointArray>::iterator it = vertices.begin(); it != vertices.end(); ++it)
    {
        printMFPA(*it);
    }
}

void visMesh::destroyMeshes() {
    DBGPRINT("Clearing geometry stores.");
    vertices.clear();
    faceCounts.clear();
    faceConnects.clear();
}

std::string visMesh::args2string(MDataBlock& data) {

	MStatus stat;
	std::string res = "";
	MString tmp = "";
	
	MDataHandle dataHandle = data.inputValue(initFile, &stat);
	CHECKERRORNORET(stat, "Unable to get initFile dataHandle in arg2string.");
	tmp = tmp + dataHandle.asString();

	for (unsigned int i = 0 ; i < simAttrs.length() ; i++) {
		dataHandle = data.inputValue(simAttrs[i],&stat);
		CHECKERRORNORET(stat, "Unable to retrieve simulator attribute[" << i << "] in args2string.");
		tmp = tmp + dataHandle.asDouble();
	}

	DBGPRINT("args2string = " << tmp);
	res = tmp.asUTF8();

	return res;
}

void visMesh::buildCube() {
    MFloatPointArray* v = new MFloatPointArray();
    MIntArray* fc1 = new MIntArray();
    MIntArray* fc2 = new MIntArray();
    float cubeSize = 1;
    const int numFaces = 6;
    const int numFaceConnects = 24;
    int face_counts[numFaces] = { 4, 4, 4, 4, 4, 4 };
    int face_connects[ numFaceConnects] = { 0, 1, 2, 3,
										    4, 5, 6, 7,
										    3, 2, 6, 5,
										    0, 3, 5, 4,
										    0, 4, 7, 1,
										    1, 7, 6, 2	};

    MFloatPoint vtx_1( -cubeSize, -cubeSize, -cubeSize );
	MFloatPoint vtx_2(  cubeSize, -cubeSize, -cubeSize );
	MFloatPoint vtx_3(  cubeSize, -cubeSize,  cubeSize );
	MFloatPoint vtx_4( -cubeSize, -cubeSize,  cubeSize );
	MFloatPoint vtx_5( -cubeSize,  cubeSize, -cubeSize );
	MFloatPoint vtx_6( -cubeSize,  cubeSize,  cubeSize );
	MFloatPoint vtx_7(  cubeSize,  cubeSize,  cubeSize );
	MFloatPoint vtx_8(  cubeSize,  cubeSize, -cubeSize );
	v->append( vtx_1 );
	v->append( vtx_2 );
	v->append( vtx_3 );
	v->append( vtx_4 );
	v->append( vtx_5 );
	v->append( vtx_6 );
	v->append( vtx_7 );
	v->append( vtx_8 );

    for (int i = 0 ; i < numFaces ; i++) {
        fc1->append(face_counts[i]);
    }
    for (int i = 0 ; i < numFaceConnects ; i++) {
        fc1->append(face_connects[i]);
    }

    vertices.push_back(*v);
    faceCounts.push_back(*fc1);
    faceConnects.push_back(*fc2);
}

#pragma endregion "Small helper methods"

MObject visMesh::getMesh(const MTime& time, MObject& outData, MStatus& stat) {
    MFnMesh	meshFS;
    MFloatPointArray verts2;
    MIntArray faceCounts2;
    MIntArray faceConnects2;

    // If the simulator is missing , create a default mesh.
    if (simulator == NULL) {
        buildCube();
    }
	// Simulator must work, so lets use that instead.
	int t = ((int) time.value())-1; // Maya have frame=1 as first frame, correct for it.
    DBGPRINT("getMesh: getting mesh no " << t+1 << ". Currently storing " << vertices.size() << " meshes.");
	if (t < vertices.size()) {
        cout << "visMesh: Constructing mesh no " << t+1 << ". Currently storing " << vertices.size() << " meshes." << endl;
        verts2 = vertices[t];
		faceCounts2 = faceCounts[t];
		faceConnects2 = faceConnects[t];
	}
	else {
		DBGPRINT("getMesh: Adjust sim length to access frame.");
	}

    MObject newMesh = meshFS.create(verts2.length(),
                                    faceCounts2.length(),
                                    verts2,
                                    faceCounts2,
                                    faceConnects2,
                                    outData,
                                    &stat);
    return newMesh;
}

// MPxNode overrides
MStatus visMesh::compute(const MPlug& plug, MDataBlock& data) {
    MStatus stat;
    MDataHandle dataHandle;
    MString filePath;
	MDoubleArray simArgs;

    if (plug == outputMesh) {
        DBGPRINT("compute: Entering compute for outputMesh.");

	    std::string config = args2string(data);
	    DBGPRINT("copute: configs are " << lastConfig << "/" << config);

        // Get the time.
        MDataHandle timeData = data.inputValue(time, &stat);
        CHECKERROR(stat,"Unable to get time data handle.");
        MTime time = timeData.asTime();
        int t = ((int) time.value());

        if (config != lastConfig) {
            lastConfig = config;
            // resim everything until selected frame.
            dataHandle = data.inputValue(initFile, &stat);
		    CHECKERROR(stat, "Unable to retrieve initFile handle for simulation.");
		    filePath = dataHandle.asString();

		    for (unsigned int i = 0 ; i < simAttrs.length() ; i++) {
			    dataHandle = data.inputValue(simAttrs[i],&stat);
			    CHECKERROR(stat, "Unable to retrieve simulator attribute[" << i << "] for simulation.");
			    simArgs.append(dataHandle.asDouble());
		    }

		    DBGPRINT("compute: Initializing simulator.");
		    // (re)Initialize simulator.
		    simulator->initialize(filePath, simArgs);
		
		    // Clear geometry buffers and simulate for entire length.
            destroyMeshes();

		    for (int i = 0 ; i < t ; i++) {
                int step = simulator->getTime();
                DBGPRINT("Saving step " << step);
                MFloatPointArray verts = MFloatPointArray(simulator->getMeshVerts());
                MIntArray fcount = MIntArray(simulator->getMeshFaceCounts());
                MIntArray fcons = MIntArray(simulator->getMeshFaceConnects());

                vertices.push_back(verts);
                faceCounts.push_back(fcount);
                faceConnects.push_back(fcons);

                DBGPRINT("Simulator going from step " << step << " to " << step+1 << ".");
                simulator->tick();
		    }
        }
        else {
            if (vertices.size() < t) {
                // sim until we have the desired frame
                while (simulator->getTime() < t) {
                    int step = simulator->getTime();
                    DBGPRINT("Saving step " << step);
                    MFloatPointArray verts = MFloatPointArray(simulator->getMeshVerts());
                    MIntArray fcount = MIntArray(simulator->getMeshFaceCounts());
                    MIntArray fcons = MIntArray(simulator->getMeshFaceConnects());

                    vertices.push_back(verts);
                    faceCounts.push_back(fcount);
                    faceConnects.push_back(fcons);

                    DBGPRINT("Simulator going from step " << step << " to " << step+1 << ".");
                    simulator->tick();
                }
            }
        }

        // Get the output object.
        MDataHandle meshHandle = data.outputValue(outputMesh, &stat);
        CHECKERROR(stat,"Unable to get the outputMesh handle.");

        MFnMeshData dataCreator;
        MObject newMesh = dataCreator.create(&stat);
        CHECKERROR(stat,"Unable to create outputData.");

        // Retrieve the mesh
	    DBGPRINT("Compute: Calling getMesh for the wanted mesh");
        //printMeshes();
        getMesh(time, newMesh, stat);
        CHECKERROR(stat,"Error creating the new mesh.");

        // Pass on the mesh through the handle and mark the node clean.
        meshHandle.set(newMesh);
        data.setClean(plug);
    }
    else {
        // We can only cmpute our outputmesh.
        return MS::kUnknownParameter;
    }
    return MS::kSuccess;
}

void* visMesh::creator() {
    return new visMesh;
}

MStatus visMesh::initialize() {
	DBGPRINT("Entering initializer");
    MFnUnitAttribute unitAttr;
    MFnTypedAttribute typedAttr;
	MFnTypedAttribute stringAttr;

    MStatus stat;

    // Inputs
    visMesh::time = unitAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0, &stat);
    CHECKERROR(stat, "Unable to create time input attribute.");

    visMesh::initFile = stringAttr.create("initFile", "if", MFnData::kString, &stat);
	CHECKERROR(stat, "Unable to create initFile input attribute.");
	stringAttr.setWritable(true);
	stringAttr.setUsedAsFilename(true);
	stringAttr.setStorable(true);

    // Outputs
    visMesh::outputMesh = typedAttr.create("outputMesh", "out", MFnData::kMesh, &stat);
    CHECKERROR(stat, "Unable to create outputMesh output attribute.");
    typedAttr.setStorable(false);

    // Add attributes
    stat = addAttribute(visMesh::time);
    CHECKERROR(stat, "Unable to add time attribute.");

	stat = addAttribute(visMesh::initFile);
	CHECKERROR(stat, "Unable to add initFile attribute.");

    stat = addAttribute(visMesh::outputMesh);
    CHECKERROR(stat, "Unable to add outputMesh attribute.");
    

    // Add affects-relationships
    stat = attributeAffects(visMesh::time, visMesh::outputMesh);
    CHECKERROR(stat, "Unable to establish affect-relationship between time and mesh.");
    stat = attributeAffects(visMesh::initFile, visMesh::outputMesh);
    CHECKERROR(stat, "Unable to establish affect-relationship between initFile and mesh.");

	DBGPRINT("Initializer: Assigning simulator.");
	simulator = new chanVeseSim();

	DBGPRINT("Initializer: Getting attribute names from simulator.");
	MStringArray argNames = *(simulator->getArguments());
	DBGPRINT("Initializer: Got names.");

    for (unsigned int i = 0 ; i < argNames.length() ; i++) {
		DBGPRINT("Initializer: Creating dynamic attribute: " << argNames[i]);
         MFnNumericAttribute attr;
         MObject newAttr;

        simAttrs.append(newAttr);
        simAttrs[i] = attr.create(argNames[i], argNames[i], MFnNumericData::kDouble, 0.0, &stat);
        CHECKERROR(stat, "Unable to create " << argNames[i] << " as attribute.");
        attr.setStorable(true);
        attr.setWritable(true);

        stat = addAttribute(simAttrs[i]);
        CHECKERROR(stat, "Unable to add " << argNames[i] << " as attribute.");
        stat = attributeAffects(simAttrs[i], visMesh::outputMesh);
        CHECKERROR(stat, "Unable to add " << argNames[i] << " to attribute affect relation.");
		DBGPRINT("Initializer: Added attribute.");
    }

    DBGPRINT("Initializing mesh stores.");
    vertices = std::vector<MFloatPointArray>();
    faceCounts = std::vector<MIntArray>();
    faceConnects = std::vector<MIntArray>();
    DBGPRINT("Building default cubeMesh.");
    buildCube();

	DBGPRINT("Initializer: Exiting.");
    return MS::kSuccess;
}

// NODE REGISTRATION AND REMOVAL
MStatus initializePlugin(MObject obj) {
    MStatus stat;
	DBGPRINT("Registering plugin information.");
    MFnPlugin plugin(obj,"Martin Jørgensen","0.2","any");

	DBGPRINT("Registering node.");
    stat = plugin.registerNode("visMesh", visMesh::id, visMesh::creator, visMesh::initialize);

    if (!stat) {
        stat.perror("registerNode");
        return stat;
    }
        
	DBGPRINT("Returning from registration.");
    return stat;
}

MStatus uninitializePlugin(MObject obj) {
    MStatus stat;
    MFnPlugin plugin(obj);

    stat = plugin.deregisterNode(visMesh::id);
    
    delete(visMesh::simulator);

    if (!stat) {
        stat.perror("deregisterNode");
        return stat;
    }
    return stat;
}