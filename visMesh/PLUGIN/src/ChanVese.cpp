#include <ChanVese.h>

using namespace dsc;

chanVeseSim::chanVeseSim() {}

MStringArray* chanVeseSim::getArguments() {
    args = MStringArray();
	args.append("Sphere-Interpolator_radius:");
    args.append("Simulator_time_step:");
	return &args;
}

void chanVeseSim::initialize(MString initFile, MDoubleArray params) {
	
    //cout << "ChanVese: Initializing simulator." << endl;
    startFile = MString(initFile);
    vertices = MFloatPointArray();
    faceCounts = MIntArray();
    faceConnects = MIntArray();
    step_length = (params[1] > 0) ? params[1] : 0.01;

    V origo = V(0, 0, 0);
    V dimensions = V(1,1,1);

	domain = ComputationalDomain<MT>();
    dinput = DSCInput<MT>();
    sphereinterpolator = SphereInterpolator<MT>();
    sphereinterpolator.set_dimensions(dimensions);
    sphereinterpolator.set_center(origo, dimensions);
    sphereinterpolator.set_radius((params[0] > 0) ? params[0] : 0.25);

    sampler = ImageSampler<MT>(sphereinterpolator);
    segmentation = Segmentation<MT>(domain, sampler, dinput);

	// TODO: Find proper way to handle the missing file at initialization.
    if (startFile == "") {
        //startFile = "D:/Dropbox/4th year/Blok 1 - BSc/dsc-repo/CODE/DSC_PROJECT/DSC/data/cube_coarse.dsc";
		//startFile = "C:/Dropbox/4th year/Blok 1 - BSc/dsc-repo/CODE/DSC_PROJECT/DSC/data/cube_coarse.dsc";
		cout << "Simulator cannot initialize without a file, please select one." << endl;
		buildCube();
		step = 0;
		return;
    }
    cout << "ChanVese: Initializing with file : " << startFile << endl;
    read_dsc_file(startFile.asUTF8(), segmentation.get_dsc());

    //cout << "ChanVese: Checking validity of initialization." << endl;
    check_validity(segmentation.get_dsc());
    check_validity(segmentation.get_dsc());
    
    //DSC<MT>& mesh = segmentation.get_dsc();
    storeMesh(segmentation.get_dsc());
    step = 0;
    
    //DSC<MT>& mesh2 = segmentation.get_dsc(); //Meshen er broken her.
	return;
}

void chanVeseSim::tick() {
    // Simulation work
    //FIXME: Hvorfor har segmentation ikke længere en mesh i sig?
    if (step_length > 0) {
        //cout << "ChanVese: Doing step with size: " << step_length << endl;
        cout << "ChanVese: simulating frame number " << (step+1) << endl;
        segmentation.time_step(step_length);
        //cout << "ChanVese: Storing step." << endl;
        storeMesh(segmentation.get_dsc());
        //cout << "ChanVese: Increaing timestamp." << endl;
        step += 1;
    }
    else {
        cout << "ChanVese ERROR: Asked to step with 0 or negative stepsize. No step was made." << endl;
    }
}

int chanVeseSim::getTime() {
    return step;
}

MFloatPointArray chanVeseSim::getMeshVerts() {
    return vertices;
}

MIntArray chanVeseSim::getMeshFaceCounts() {
    return faceCounts;
}

MIntArray chanVeseSim::getMeshFaceConnects() {
    return faceConnects;
}

void chanVeseSim::storeMesh(DSC<MT> & dsc) {

    vertices.clear();
    faceCounts.clear();
    faceConnects.clear();

    DSC<MT>::face_kernel_type::iterator fit = dsc.faces_begin();
    DSC<MT>::face_kernel_type::iterator ff_it = dsc.faces_end();
    while (fit != ff_it)
    {
        if (fit->is_interface()) {// || fit->is_boundary()) {
            // This part is quite important, DSC doesn't maintain consistent orientation of the triangles on the interface
            DSC<MT>::simplex_set_type st_f;
            dsc.star(fit.key(), st_f);
            int label = -100;
            for (DSC<MT>::simplex_set_type::tetrahedron_set_iterator tit = st_f.tetrahedra_begin(); tit != st_f.tetrahedra_end(); tit++) {
			    int tl = dsc.find_tetrahedron(*tit).label;
                if (tl > label) {
			        //std::cout << label << " " << tl << std::endl;
                    dsc.orient_faces_consistently(*tit);
			    }
                label = tl;
		    }

            std::vector<typename DSC<MT>::node_key_type> verts(3);
            dsc.vertices(fit.key(), verts);

            V n = normal_direction<MT>(dsc.find_node(verts[0]).v,
			                           dsc.find_node(verts[1]).v, 
				                       dsc.find_node(verts[2]).v);
            //dsc.normal_to_face(fit.key());

            faceCounts.append(3);
            for (int i = 0; i < 3; ++i) {
                V v = dsc.find_node(verts[i]).v;
                faceConnects.append(vertices.length());
                vertices.append(MFloatPoint(v[0], v[1], v[2]));
          }
        }
        ++fit;
      }
}

void chanVeseSim::buildCube() {
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