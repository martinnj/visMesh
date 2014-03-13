#ifndef _GMESH
#define _GMESH


#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>

// All mesh structure interfaces should implement this virtual class for the plugin to work.

class gmesh {
public:
    gmesh() {}
    gmesh(const gmesh& orig) {}
    virtual ~gmesh() {}

public:
	virtual MFloatPointArray* getVertices() const = 0;
	virtual MIntArray* getFaceCounts() const = 0;
	virtual MIntArray* getFaceConnects() const = 0;
};

#endif //_GMESH
