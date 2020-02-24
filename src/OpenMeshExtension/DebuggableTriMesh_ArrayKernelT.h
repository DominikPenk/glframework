#pragma once

#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/FinalMeshItemsT.hh>
#include <OpenMesh/Core/Mesh/TriMeshT.hh>

#include "DebuggableTriConnectivity.h"
#include "DebuggableAttribKernelT.h"

namespace OpenMesh {
template<class Traits>
class PolyMesh_ArrayKernelT;

/// Helper class to create a TriMesh-type based on ArrayKernelT
template <class Traits>
struct Debuggable_TriMesh_ArrayKernel_GeneratorT
{
	typedef FinalMeshItemsT<Traits, true>                                 MeshItems;
	typedef DebuggableAttribKernelT<MeshItems, DebuggableTriConnectivity> AttribKernel;
	typedef TriMeshT<AttribKernel>                                        Mesh;
};



/** \ingroup mesh_types_group
	Triangle mesh based on the ArrayKernel.
	\see OpenMesh::TriMeshT
	\see OpenMesh::ArrayKernelT
*/
template <class Traits = DefaultTraits>
class DebuggableTriMesh_ArrayKernelT
	: public Debuggable_TriMesh_ArrayKernel_GeneratorT<Traits>::Mesh
{
public:
	DebuggableTriMesh_ArrayKernelT() {}
	template<class OtherTraits>
	explicit DebuggableTriMesh_ArrayKernelT(const DebuggableTriMesh_ArrayKernelT<OtherTraits>& t)
	{
		//assign the connectivity and standard properties
		this->assign(t, true);
	}
};

}
