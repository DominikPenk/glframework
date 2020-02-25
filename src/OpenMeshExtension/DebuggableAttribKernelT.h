#pragma once

#include <OpenMesh/Core/Mesh/AttribKernelT.hh>

#include "DebuggableMixin.h"

namespace OpenMesh {

template <class MeshItems, class Connectivity>
class DebuggableAttribKernelT : public AttribKernelT<MeshItems, Connectivity> {
public:
	typedef typename MeshItems::VertexData                  VertexData;
	typedef typename AttribKernelT<MeshItems, Connectivity> Base;

	//-------------------------------------------------------------------- points
	const Point& point(VertexHandle _vh) const
	{
		checkBreakPoints(_vh, OpenMeshFunction::point);
		return Base::point(_vh);
	}

	Base::Point& point(VertexHandle _vh)
	{
		checkBreakPoints(_vh, OpenMeshFunction::point);
		return Base::point(_vh);
	}
	void set_point(VertexHandle _vh, const Point& _p)
	{
		checkBreakPoints(_vh, OpenMeshFunction::set_point);
		Base::set_point(_vh, _p);
	}

	//------------------------------------------------------------ vertex normals
	const Normal& normal(VertexHandle _vh) const
	{
		checkBreakPoints(_vh, OpenMeshFunction::normal);
		return Base::normal(_vh);
	}

	void set_normal(VertexHandle _vh, const Normal& _n)
	{
		checkBreakPoints(_vh, OpenMeshFunction::normal);

		Base::set_normal(_vh, _n);
	}

	//------------------------------------------------------------- vertex colors

	const Color& color(VertexHandle _vh) const
	{
		checkBreakPoints(_vh, OpenMeshFunction::color);
		return Base::color(_vh);
	}

	void set_color(VertexHandle _vh, const Color& _c)
	{
		checkBreakPoints(_vh, OpenMeshFunction::set_color);
		Base::set_color(_vh, _c);
	}


	//------------------------------------------------------- vertex 1D texcoords

	const TexCoord1D& texcoord1D(VertexHandle _vh) const {
		checkBreakPoints(_vh, OpenMeshFunction::texcoord1D);
		return Base::texcoord1D(_vh);
	}

	void set_texcoord1D(VertexHandle _vh, const TexCoord1D& _t) {
		checkBreakPoints(_vh, OpenMeshFunction::set_texcoord1D);
		Base::set_texcoord1D(_vh, _t);
	}


	//------------------------------------------------------- vertex 2D texcoords

	const TexCoord2D& texcoord2D(VertexHandle _vh) const {
		checkBreakPoints(_vh, OpenMeshFunction::texcoord2D);
		return Base::texcoord2D(_vh);
	}

	void set_texcoord2D(VertexHandle _vh, const TexCoord2D& _t) {
		checkBreakPoints(_vh, OpenMeshFunction::set_texcoord2D);
		Base::set_texcoord2D(_vh, _t);
	}


	//------------------------------------------------------- vertex 3D texcoords

	const TexCoord3D& texcoord3D(VertexHandle _vh) const {
		checkBreakPoints(_vh, OpenMeshFunction::texcoord3D);
		return Base::texcoord3D(_vh);
	}

	void set_texcoord3D(VertexHandle _vh, const TexCoord3D& _t) {
		checkBreakPoints(_vh, OpenMeshFunction::set_texcoord3D);
		Base::set_texcoord3D(_vh, _t);
	}

	//.------------------------------------------------------ halfedge 1D texcoords

	const TexCoord1D& texcoord1D(HalfedgeHandle _heh) const {
		checkBreakPoints(_heh, OpenMeshFunction::texcoord1D);
		return Base::texcoord1D(_heh);
	}

	void set_texcoord1D(HalfedgeHandle _heh, const TexCoord1D& _t) {
		checkBreakPoints(_heh, OpenMeshFunction::set_texcoord1D);
		Base::set_texcoord1D(_heh, _t);
	}


	//------------------------------------------------------- halfedge 2D texcoords

	const TexCoord2D& texcoord2D(HalfedgeHandle _heh) const {
		checkBreakPoints(_heh, OpenMeshFunction::texcoord2D);
		return Base::texcoord2D(heh);
	}

	void set_texcoord2D(HalfedgeHandle _heh, const TexCoord2D& _t) {
		checkBreakPoints(_heh, OpenMeshFunction::set_texcoord2D);
		Base::set_texcoord2D(_heh, _t);
	}


	//------------------------------------------------------- halfedge 3D texcoords
	
	const TexCoord3D& texcoord3D(HalfedgeHandle _heh) const {
		checkBreakPoints(_heh, OpenMeshFunction::texcoord3D);
		return Base::texcoord3D(_heh);
	}

	void set_texcoord3D(HalfedgeHandle _heh, const TexCoord3D& _t) {
		checkBreakPoints(_heh, OpenMeshFunction::set_texcoord3D);
		Base::set_texcoord3D(_heh, _t);
	}

	//------------------------------------------------------------- edge colors

	const Color& color(EdgeHandle _eh) const
	{
		checkBreakPoints(_eh, OpenMeshFunction::color);
		return Base::color(_eh);
	}

	void set_color(EdgeHandle _eh, const Color& _c)
	{
		checkBreakPoints(_eh, OpenMeshFunction::set_color);
		Base::set_color(_eh, _c);
	}


	//------------------------------------------------------------- halfedge normals

	const Normal& normal(HalfedgeHandle _heh) const
	{
		checkBreakPoints(_heh, OpenMeshFunction::normal);
		return Base::normal(_heh);
	}

	void set_normal(HalfedgeHandle _heh, const Normal& _n)
	{
		checkBreakPoints(_heh, OpenMeshFunction::set_normal);
		Base::set_normal(_heh, _n);
	}


	//------------------------------------------------------------- halfedge colors

	const Color& color(HalfedgeHandle _heh) const
	{
		checkBreakPoints(_heh, OpenMeshFunction::color);
		return Base::color(_heh);
	}

	void set_color(HalfedgeHandle _heh, const Color& _c)
	{
		checkBreakPoints(_heh, OpenMeshFunction::set_color);
		Base::set_color(_heh, _c);
	}

	//-------------------------------------------------------------- face normals

	const Normal& normal(FaceHandle _fh) const
	{
		checkBreakPoints(_fh, OpenMeshFunction::normal);
		return Base::normal(_fh);
	}

	void set_normal(FaceHandle _fh, const Normal& _n)
	{
		checkBreakPoints(_fh, OpenMeshFunction::set_normal);
		Base::set_normal(_fh, _n);
	}

	//-------------------------------------------------------------- per Face Texture index

	const TextureIndex& texture_index(FaceHandle _fh) const
	{
		checkBreakPoints(_fh, OpenMeshFunction::texture_index);
		return Base::texture_index(_fh);
	}

	void set_texture_index(FaceHandle _fh, const TextureIndex& _t)
	{
		checkBreakPoints(_fh, OpenMeshFunction::set_texture_index);
		Base::set_texture_index(_fh, _t);
	}

	//--------------------------------------------------------------- face colors

	const Color& color(FaceHandle _fh) const
	{
		checkBreakPoints(_fh, OpenMeshFunction::color);
		return Base::color(_fh);
	}

	void set_color(FaceHandle _fh, const Color& _c)
	{
		checkBreakPoints(_fh, OpenMeshFunction::set_color);
		Base::set_color(_fh, _c);
	}

	VertexData& data(VertexHandle _vh)
	{
		checkBreakPoints(_vh, OpenMeshFunction::data);
		return Base::data(_vh);
	}

	const VertexData& data(VertexHandle _vh) const
	{
		checkBreakPoints(_vh, OpenMeshFunction::const_data);
		return Base::data(_vh);
	}

	FaceData& data(FaceHandle _fh)
	{
		checkBreakPoints(_fh, OpenMeshFunction::data);
		return Base::data(_fh);
	}

	const FaceData& data(FaceHandle _fh) const
	{
		checkBreakPoints(_fh, OpenMeshFunction::const_data);
		return Base::data(_fh);
	}

	EdgeData& data(EdgeHandle _eh)
	{
		checkBreakPoints(_eh, OpenMeshFunction::data);
		return Base::data(_eh);
	}

	const EdgeData& data(EdgeHandle _eh) const
	{
		checkBreakPoints(_eh, OpenMeshFunction::const_data);
		return Base::data(_eh);
	}

	HalfedgeData& data(HalfedgeHandle _heh)
	{
		checkBreakPoints(_heh, OpenMeshFunction::data);
		return Base::data(_heh);
	}

	const HalfedgeData& data(HalfedgeHandle _heh) const
	{
		checkBreakPoints(_heh, OpenMeshFunction::const_data);
		return Base::data(_heh);
	}
};
}