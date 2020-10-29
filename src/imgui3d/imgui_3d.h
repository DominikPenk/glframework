#pragma once
#include <memory>
#include <unordered_map>

#include <imgui.h>

#include <glm/glm.hpp>

#include "buffers.hpp"
#include "shadermanager.hpp"
#include "texture.hpp"
#include "draw_batch.hpp"

namespace gl {
	class Renderer;
	
}

namespace ImGui3D {
	struct ImGui3DContext;

	extern std::shared_ptr<ImGui3DContext> GImGui3D;

	struct DrawData {
		static constexpr GLenum enum_type = GL_FLOAT;
		DrawData(glm::vec4 position, glm::vec4 color, glm::vec4 id, glm::vec2 uv) {
			payload[0] = position.x;
			payload[1] = position.z;
			payload[2] = position.y;
			payload[3] = position.w;

			payload[4] = color.r;
			payload[5] = color.g;
			payload[6] = color.b;
			payload[7] = color.a;

			payload[8] = id.r;
			payload[9] = id.g;
			payload[10] = id.b;
			payload[11] = id.a;

			payload[12] = uv.x;
			payload[13] = uv.y;
		}
		float payload[14];
	};

	struct DrawCommand {
		std::shared_ptr<gl::CompactVertexBufferObject<glm::vec4, glm::vec4, ImGuiID, glm::vec2>> data;
		gl::DrawBatch batch;
		gl::Shader shader;

		void execute();

		DrawCommand();

		void Clear() {
			data->clear();
			batch.indexBuffer->clear();
		}

		void AddFilledScreenAlignedQuad(glm::vec4 pos, ImVec2 size, glm::vec4 color, ImGuiID id = 0);
		void AddFilledScreenAlignedQuad(glm::vec3 pos, ImVec2 size, glm::vec4 color, ImGuiID id = 0);
		
		void AddScreenAlignedQuad(glm::vec4 pos, ImVec2 size, float width, glm::vec4 color, ImGuiID id = 0);

		void AddFilledScreenAlignedCircle(glm::vec4 pos, float radius, glm::vec4 color, unsigned int segments, ImGuiID id = 0);
		void AddFilledScreenAlignedCircle(glm::vec3 pos, float radius, glm::vec4 color, unsigned int segments, ImGuiID id = 0);

		void AddScreenAlignedCircle(glm::vec4 pos, float radius, float width, glm::vec4 color, unsigned int segments, ImGuiID id = 0);
		void AddScreenAlignedCircle(glm::vec3 pos, float radius, float width, glm::vec4 color, unsigned int segments, ImGuiID id = 0);

		void AddDashedCircle(glm::vec4 pos, float radius, float width, glm::vec4 color, unsigned int segments, unsigned int subdivisions, ImGuiID id = 0);
		void AddDashedCircle(glm::vec3 pos, float radius, float width, glm::vec4 color, unsigned int segments, unsigned int subdivisions, ImGuiID id = 0);

		void AddTriangle(glm::vec4 a, glm::vec4 b, glm::vec4 c, glm::vec4 color, ImGuiID id = 0);
		void AddTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color, ImGuiID id = 0);

		void AddTriangleStrip(std::vector<glm::vec4> points, std::vector<glm::vec4> colors, glm::mat4 T = glm::mat4(1), ImGuiID id = 0);
		void AddTriangleStrip(std::vector<glm::vec3> points, std::vector<glm::vec4> colors, glm::mat4 T = glm::mat4(1), ImGuiID id = 0);
		
		void AddTriangleStrip(std::vector<glm::vec4> points, glm::vec4 color, glm::mat4 T = glm::mat4(1), ImGuiID id = 0);
		void AddTriangleStrip(std::vector<glm::vec3> points, glm::vec4 color, glm::mat4 T = glm::mat4(1), ImGuiID id = 0);

		void AddCylinder(glm::vec4 from, glm::vec4 to, float radius, glm::vec4 color, int segments, bool close_bottom, bool close_top, ImGuiID id = 0);
		void AddCylinder(glm::vec3 from, glm::vec3 to, float radius, glm::vec4 color, int segments, bool close_bottom, bool close_top, ImGuiID id = 0);
		
		void AddCone(glm::vec4 from, glm::vec4 to, float radius, glm::vec4 color, int segments, bool close, ImGuiID id = 0);
		void AddCone(glm::vec3 from, glm::vec3 to, float radius, glm::vec4 color, int segments, bool close, ImGuiID id = 0);

		void AddArrow(glm::vec4 from, glm::vec4 to, glm::vec4 color, float r1, float r2, float tip = .25f, int segments = 32, ImGuiID id = 0);
		void AddArrow(glm::vec3 from, glm::vec3 to, glm::vec4 color, float r1, float r2, float tip = .25f, int segments = 32, ImGuiID id = 0);

		void AddPolyLine(std::vector<glm::vec4> points, glm::vec4 color, float width, bool closed, glm::mat4 T = glm::mat4(1), ImGuiID id = 0);
		void AddPolyLine(std::vector<glm::vec3> points, glm::vec4 color, float width, bool closed, glm::mat4 T = glm::mat4(1), ImGuiID id = 0);

		void AddLine(glm::vec4 from, glm::vec4 to, float width, glm::vec4 color, ImGuiID id = 0);
		void AddLine(glm::vec3 from, glm::vec3 to, float width, glm::vec4 color, ImGuiID id = 0);

		void AddFilledSemiCircle(glm::vec4 pos, glm::vec4 axis, float radius, float startAngle, float endAngle, glm::vec4 color, int segments, ImGuiID id = 0);
		void AddFilledSemiCircle(glm::vec3 pos, glm::vec3 axis, float radius, float startAngle, float endAngle, glm::vec4 color, int segments, ImGuiID id = 0);
		
		void AddCircle(glm::vec4 pos, glm::vec4 axis, float radius, float width, glm::vec4 color, int segments, ImGuiID id = 0);
		void AddCircle(glm::vec3 pos, glm::vec3 axis, float radius, float width, glm::vec4 color, int segments, ImGuiID id = 0);
		
		void AddFilledArc(glm::vec4 pos, glm::vec4 from, glm::vec4 to, float radius, glm::vec4 color, int segments, ImGuiID id = 0);
		void AddFilledArc(glm::vec3 pos, glm::vec3 from, glm::vec3 to, float radius, glm::vec4 color, int segments, ImGuiID id = 0);
		
		void AddFilledSemiCircle(glm::vec4 pos, glm::vec4 startDirection, glm::vec4 axis, float angle, glm::vec4 color, int segments, ImGuiID id = 0);
		void AddFilledSemiCircle(glm::vec3 pos, glm::vec3 startDirection, glm::vec3 axis, float angle, glm::vec4 color, int segments, ImGuiID id = 0);

		void AddFilledAxisAlignedCube(glm::vec4 pos, glm::vec3 size, glm::vec4 color, ImGuiID id = 0);
		void AddFilledAxisAlignedCube(glm::vec3 pos, glm::vec3 size, glm::vec4 color, ImGuiID id = 0);

		void AddAxisAlignedCube(glm::vec4 pos, glm::vec3 size, float thickness, glm::vec4 color, ImGuiID id = 0);
		void AddAxisAlignedCube(glm::vec3 pos, glm::vec3 size, float thickness, glm::vec4 color, ImGuiID id = 0);
	
		void AddIndexedFaceSet(const std::vector<glm::vec4>& vertices, const std::vector<unsigned int>& indices, glm::vec4 color, ImGuiID id = 0);

		// Screen space functions
		void AddFilledScreenSpaceCircle(ImVec2 pos, float radius, glm::vec4 color, unsigned int segments, float depth = -1.f, ImGuiID id = 0);
		void AddScreenSpaceCircle(ImVec2 pos, float radius, float width, glm::vec4 color, unsigned int segments, float depth = -1.f, ImGuiID id = 0);
		void AddScreenSpaceLine(ImVec2 from, ImVec2 to, float width, glm::vec4 color, float depth = -1.f, ImGuiID id = 0);
	};

	enum ImGui3DColors {
		ImGui3DCol_xAxis,
		ImGui3DCol_xAxis_selected,
		ImGui3DCol_yAxis,
		ImGui3DCol_yAxis_selected,
		ImGui3DCol_zAxis,
		ImGui3DCol_zAxis_selected,
		ImGui3DCol_yzPlane,
		ImGui3DCol_yzPlane_selected,
		ImGui3DCol_xzPlane,
		ImGui3DCol_xzPlane_selected,
		ImGui3DCol_xyPlane,
		ImGui3DCol_xyPlane_selected,
		ImGui3DCol_xRotation_circle,
		ImGui3DCol_yRotation_circle,
		ImGui3DCol_zRotation_circle,
		ImGui3DCol_Vertex,
		ImGui3DCol_Vertex_selected,

		// Editor Widgets
		ImGui3DCol_Editor_Widget,
		ImGui3DCol_Editor_Widget_selected,

		// Surface Widgets
		ImGui3DCol_Grid,

		ImGui3DCol_COUNT
	};

	struct ImGui3DStyle {
		// Gizmo settings
		float      GizmoSize;
		float      VertexSize;
		int        RotationGizmoSegments;
		int        TranslationGizmoSegments;
		float      TranslationGizmoInnerPadding;
		float      TranslationGizmoPlaneSize;

		// Editor widget settings
		float      LightRadius;
		float      LightDirectionLength;
		float      SpotLightConeHeight;
		float      LightHandleSize;
		float      CubeMapRadius;
		float      CubeMapSize;
		
		// Surface settings
		float      GridLineWidth;


		glm::vec4  Colors[ImGui3DCol_COUNT];

		/// <summary>Returns the default or selected color for the element</summary>
		/// <param name="id">Id of the object</param>
		/// <param name="element">Element type to retreive the color</param>
		/// <returns>Either the default or selected color of the element</returns>
		glm::vec4 getColorForID(ImGuiID id, ImGui3DColors element);

		/// <summary>Returns the default or selected color for a given element</summary>
		/// <param name="element">Element type to retreive the color</param>
		/// <returns>Either the default or selected color of the element</returns>
		glm::vec4 getColor(ImGui3DColors element);

		// Default style
		ImGui3DStyle();
	};

	struct ImGui3DContext {
		bool                               KeepCaptureFocus;
		ImGuiID                            ActiveId;
		ImGuiID	                           ActiveIdPreviousFrame;
		ImGuiID                            CurrentId;
		glm::uvec4                         ActiveIdColor;

		glm::mat4                          ModelMatrix;
		glm::mat4                          ViewMatrix;
		glm::mat4                          ProjectionMatrix;
		glm::mat4                          ViewProjectionMatrix;
		glm::mat4                          ViewPerspectiveMatrixInverse;
		glm::vec4                          CameraPosition;
		ImVec2                             ScreenSize;
		float                              Aspect;

		ImGui3DStyle                       Style;

		gl::Shader                         Shader;
		std::function<glm::uvec4(ImVec2)>  GetHoveredIdImpl;
		std::vector<ImGuiID>               SeedStack;

		std::vector<std::shared_ptr<DrawCommand>> drawCommands;
		std::shared_ptr<DrawCommand> currentDrawList() {
			return drawCommands.back();
		}

		ImGui3DContext();
	};



	/// <summary>Create a new context for ImGui3D</summary>
	/// <returns>The newly created context</returns>
	std::shared_ptr<ImGui3DContext> CreateContext();

	/// <summary>Set the global ImGui3D context</summary>
	/// <param name="ctx">Pointer to the context to set</param>
	void SetContext(std::shared_ptr<ImGui3DContext> ctx);

	void NewFrame(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix);

	void Render();

	bool IsItemActive();

	bool IsItemHovered();

	bool IsItemClicked(int button = 0);

	/// <summary>Get an id based on a string label</summary>
	/// <param name="str">String used to generate the id</param>
	/// <param name="keepFocus">If true the generated id is kept during dragging events even if the cursor stops hovering the 3D-object</param>
	/// <returns>The generated id</returns>
	ImGuiID GetID(const std::string& str, bool keepFocus = true);

	/// <summary>Get an id based on a generic pointer</summary>
	/// <param name="str">String used to generate the id</param>
	/// <param name="keepFocus">If true the generated id is kept during dragging events even if the cursor stops hovering the 3D-object</param>
	/// <returns>The generated id</returns>
	ImGuiID GetID(const void* data, bool keepFocus = true);

	/// <summary>Get an id based on an unsigned 64 bit integer</summary>
	/// <param name="i">Integer used to generate the id</param>
	/// <param name="keepFocus">If true the generated id is kept during dragging events even if the cursor stops hovering the 3D-object</param>
	/// <returns>The generated id</returns>
	ImGuiID GetID(uint64_t i, bool keepFocus = true);

	/// <summary>Converts a color to an id</summary>
	/// <remarks>This function is more or less depricated since the shader converts the id to a color</remarks>
	/// <param name="r">Red value of the color in range [0, 256]</param>
	/// <param name="g">Green value of the color in range [0, 256]</param>
	/// <param name="b">Blue value of the color in range [0, 256]</param>
	/// <returns>The id represented by the color [r, g, b]</returns>
	ImGuiID ColorToID(unsigned char r, unsigned char g, unsigned char b);

	/// <summary>Converts an id to a color</summary>
	/// <param name="id">Id to convert</param>
	/// <returns>A color [r, g, b, a] representing the given id</returns>
	glm::vec4 IDToColor(ImGuiID id);

	/// <summary>Pushes a new seed for Id generation</summary>
	/// <remarks>Use this to distinguish between ids with similar label or pointer</remarks>
	/// <param name="id">Id used to generate the seed</param>
	void PushID(ImGuiID id);

	/// <summary>Pushes a new seed for Id generation</summary>
	/// <remarks>Use this to distinguish between ids with similar label or pointer</remarks>
	/// <param name="id">Pointer used to generate the seed</param>
	template<typename T>
	void PushID(T* id) {
		ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
		const ImGuiID seed = GetID(id, false);
		g.SeedStack.push_back(seed);
	}

	/// <summary>Pops a seed for Id generation. This should always be called when PusID was called</summary>
	void PopID();

	void TakeIOFocus(ImGuiID id);

	void ReleaseIOFocus(ImGuiID id);

	void ViewFrustum(float VP[16], glm::vec3 color);
	void ViewFrustum(glm::mat4& VP, glm::vec3 color);

	// Higher level functions
	bool RotationGizmo(const float pos[3], float angles[3], ImGuiID idoverride = 0);
	bool RotationGizmo(glm::vec3 pos, glm::vec3& angles);
	bool RotationGizmo(glm::vec4 pos, glm::vec3& angles);
	
	bool TranslationGizmo(float pos[3]);
	bool TranslationGizmo(glm::vec3& pos);
	bool TranslationGizmo(glm::vec4& pos);
	
	bool TransformGizmo(float T[16]);
	bool TransformGizmo(glm::mat4& T);

	bool Vertex(float pos[3]);
	bool Vertex(glm::vec3& pos);
	bool Vertex(glm::vec4& pos);
	
	bool RestrictedVertex(float pos[3], const float axis[3]);
	bool RestrictedVertex(glm::vec3& pos, glm::vec3 axis);
	bool RestrictedVertex(glm::vec4& pos, glm::vec4 axis);
	
	bool RestrictedVertex(float pos[3], const float axis[3], const glm::mat4 T);
	bool RestrictedVertex(glm::vec3& pos, glm::vec3 axis, glm::mat4 T);
	bool RestrictedVertex(glm::vec4& pos, glm::vec4 axis, glm::mat4 T);
}