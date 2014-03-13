#ifndef _CHANVESESIM
#define _CHANVESESIM

// Simulator that should call the Chan-Vese library and simulate that vie tha DSC mesh.

#include <gsim.h>

#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MDoubleArray.h>

// TODO: CLean this out.
#include <dsc.h>
#include <dsc_macros.h>
#include <dsc_GEL_types.h>
#include <dsc_smoothing.h>
#include <dsc_vertex_insertion.h>
#include <dsc_validation.h>
#include <dsc_topological_pass.h>
#include <dsc_detail_control.h>
#include <dsc_mesh_io.h>

#include <chan_vese_impl.h>

#include <spline_interpolator_impl.h>
#include <sphere_interpolator_impl.h>
#include <box_interpolator_impl.h>


class chanVeseSim : public gsim {

    typedef dsc::GELTypes MT;
    typedef MT::real_type                              T;
    typedef MT::vector3_type                           V;

public:
	         chanVeseSim();
	virtual ~chanVeseSim() {};

	virtual MStringArray* getArguments();
	void          initialize(MString initFile, MDoubleArray params);
	void          tick();
    int           getTime();
    MFloatPointArray getMeshVerts();
    MIntArray getMeshFaceCounts();
    MIntArray getMeshFaceConnects();

public:
    void storeMesh(dsc::DSC<MT> & dsc);
	void buildCube();

    MStringArray args;
    int step;
    double step_length;
    MString startFile;

	SphereInterpolator<MT> sphereinterpolator;
	ImageSampler<MT> sampler;
    ComputationalDomain<MT> domain;
    DSCInput<MT> dinput;
    Segmentation<MT> segmentation;

    MFloatPointArray vertices;
    MIntArray faceCounts;
    MIntArray faceConnects;

};

#endif //_CHANVESESIM