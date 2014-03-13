/* This is a sample implementation of a mesh, to test the abstract class */
#include <cubeMesh.h>

#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>

/*MFloatPointArray vertices;
MIntArray        faceCounts;
MIntArray        faceConnects;
*/
cubeMesh::cubeMesh() {
    // Whatever is needed to initialize the mesh.
    // Consider if a file might be suitable here, for hot-starting the mesh.
    // This mesh will just initialize itself into a cube

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
	vertices.append( vtx_1 );
	vertices.append( vtx_2 );
	vertices.append( vtx_3 );
	vertices.append( vtx_4 );
	vertices.append( vtx_5 );
	vertices.append( vtx_6 );
	vertices.append( vtx_7 );
	vertices.append( vtx_8 );

    for (int i = 0 ; i < numFaces ; i++) {
        faceCounts.append(face_counts[i]);
    }
    for (int i = 0 ; i < numFaceConnects ; i++) {
        faceConnects.append(face_connects[i]);
    }
}
/*
MFloatPointArray cubeMesh::getvertices() const {
    return vertices;
}

MIntArray cubeMesh::getfaceCounts() const {
    return faceCounts;
}

MIntArray cubeMesh::getfaceConnects() const {
    return faceConnects;
}
*/