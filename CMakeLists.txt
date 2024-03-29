cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(GL-Framework LANGUAGES CXX)

set(WITH_ASSIMP ON CACHE BOOL "Build with assimp")
set(WITH_OPENMESH ON CACHE BOOL "Build with OpenMesh")
set(WITH_EGL ON CACHE BOOL "Build offscreenrendering with EGL")
option(BUILD_FRAMEWORK_SAMPLES "Build framework samples" OFF)

# Find Opengl libs
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Find OpenMesh (if requested)
if(${WITH_OPENMESH})
	find_package(OpenMesh QUIET)
	if(NOT ${OpenMesh_FOUND})
		message(STATUS "Did not find OpenMesh, turning it off")
		set(WITH_OPENMESH OFF CACHE BOOL "Build with OpenMesh" FORCE)
	endif()
endif()

if(${WITH_ASSIMP})
	find_package(ASSIMP QUIET)
	if(NOT ${ASSIMP_FOUND})
		message(STATUS "Did not find Assimp, turning off")
		set(WITH_ASSIMP OFF CACHE BOOL "Build with assimp" FORCE)
	endif()
endif()

if(${WITH_EGL})
	if(NOT ${OpenGL_EGL_FOUND})
		message(STATUS "Did not find EGL, turning it off")
		set(WITH_EGL OFF CACHE BOOL "Build offscreenrendering with EGL" FORCE)
	endif()
endif()

set(INCLUDE_DIR include/glpp)

# Gather Imgui
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/imgui)
set(IMGUI_PLOT_DIR ${PROJECT_SOURCE_DIR}/implot)
# Set the imgui sources
file(GLOB IMGUI_SRC ${IMGUI_DIR}/*.h ${IMGUI_DIR}/*.cpp)
# Add glfw impl
set(IMGUI_SRC 
	${IMGUI_SRC}
	# Implementation
	${IMGUI_DIR}/backends/imgui_impl_opengl3.h
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
	${IMGUI_DIR}/backends/imgui_impl_glfw.h
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	# ImGuiPlot
	${IMGUI_PLOT_DIR}/implot.h
	${IMGUI_PLOT_DIR}/implot.cpp
	${IMGUI_PLOT_DIR}/implot_internal.h
	${IMGUI_PLOT_DIR}/implot_items.cpp
	# Custom extensions
	${INCLUDE_DIR}/imgui.hpp
	${INCLUDE_DIR}/imgui_ext.hpp
	${INCLUDE_DIR}/imgui_ext.inl.hpp
	src/imgui_ext.cpp
	${INCLUDE_DIR}/imgui_statistics.hpp
	src/imgui_statistics.cpp
	${INCLUDE_DIR}/imgui_vertical_tabs.hpp
	src/imgui_vertical_tabs.cpp
	${INCLUDE_DIR}/imgui_filesystem.hpp
	src/imgui_filesystem.cpp)

set(MESH_FILES
	${INCLUDE_DIR}/meshes.hpp
	${INCLUDE_DIR}/meshes/mesh.hpp
	src/meshes/mesh.cpp
	${INCLUDE_DIR}/meshes/triangle_mesh.hpp
	src/meshes/triangle_mesh.cpp
	${INCLUDE_DIR}/meshes/coordinate_frame.hpp
	src/meshes/coordinate_frame.cpp
	${INCLUDE_DIR}/meshes/splinecurves.hpp
	src/meshes/splinecurves.cpp
	${INCLUDE_DIR}/meshes/pointcloud.hpp
	src/meshes/pointcloud.cpp)

set(RENDERER_FILES
	${INCLUDE_DIR}/context.hpp
	${INCLUDE_DIR}/renderer.hpp
	src/context.cpp
	# Editor renderer
	${INCLUDE_DIR}/renderers/editor.hpp
	src/renderers/editor.cpp
	${INCLUDE_DIR}/renderers/editorwindow.hpp
	src/renderers/editorwindow.cpp
	# Offscreen renderer
	${INCLUDE_DIR}/renderers/offscreen_renderer.hpp
	src/renderers/offscreen_renderer.cpp
	# Immediate renderer
	${INCLUDE_DIR}/renderers/immediate_renderer.hpp
	src/renderers/immediate_renderer.cpp
)

set(EVENT_FILES
	${INCLUDE_DIR}/eventsystem.hpp
	${INCLUDE_DIR}/eventsystem.cpp
)

set(IMGUI_3D_FILES
	${INCLUDE_DIR}/imgui3d/imgui_3d.h
	${INCLUDE_DIR}/imgui3d/imgui_3d_utils.h
	src/imgui3d/imgui_3d_utils.cpp
	${INCLUDE_DIR}/imgui3d/imgui_3d_geometry.h
	src/imgui3d/imgui_3d.cpp
	src/imgui3d/imgui_3d_widgets.cpp
	${INCLUDE_DIR}/imgui3d/imgui_3d_editor_widgets.h
	src/imgui3d/imgui_3d_editor_widgets.cpp
	${INCLUDE_DIR}/imgui3d/imgui_3d_surfaces.h
	src/imgui3d/imgui_3d_surfaces.cpp
	src/imgui3d/imgui_3d_draw.cpp)

set(IMGUI_FILES ${IMGUI_SRC})

set(LOGGING_FILES 
	${INCLUDE_DIR}/logging.hpp
	src/logging.cpp)

file(GLOB STATIC_SHADERS ${PROJECT_SOURCE_DIR}/shaders/*.glsl.h)

set(GL_FILES
	${MESH_FILES}
	${RENDERER_FILES}
	${LOGGING_FILES}
	${EVENT_FILES}
	${STATIC_SHADERS}
	${INCLUDE_DIR}/texture.hpp
	src/texture.cpp
	src/paged_texture.cpp
	${INCLUDE_DIR}/framebuffer.hpp
	src/framebuffer.cpp
	${INCLUDE_DIR}/draw_batch.hpp
	${INCLUDE_DIR}/draw_batch.inl.hpp
	src/draw_batch.cpp
	${INCLUDE_DIR}/intermediate.h
	${INCLUDE_DIR}/intermediate.inl.h
	src/intermediate.cpp
	${INCLUDE_DIR}/variadic_buffer.hpp
	${INCLUDE_DIR}/map_buffer.hpp
	${INCLUDE_DIR}/shader_storage_buffer.hpp
	${INCLUDE_DIR}/shader_storage_buffer.cpp
	${INCLUDE_DIR}/shadermanager.hpp
	${INCLUDE_DIR}/shadermanager.inl.hpp
	src/shadermanager.cpp
	${INCLUDE_DIR}/controls.hpp
	src/controls.cpp
	# ${INCLUDE_DIR}/offscreen_renderer.hpp
	# src/offscreen_renderer.cpp
	${INCLUDE_DIR}/camera.hpp
	${INCLUDE_DIR}/buffers.hpp
	src/buffers.cpp
	${INCLUDE_DIR}/gl_internal.hpp
	# ${INCLUDE_DIR}/uiwindow.hpp
	# src/uiwindow.cpp
	${INCLUDE_DIR}/IconsFontAwesome5.h)

	
source_group("Meshes" FILES ${MESH_FILES})
source_group("Dear Imgui" FILES ${IMGUI_FILES})
source_group("Dear Imgui 3D" FILES ${IMGUI_3D_FILES})
source_group("Renderes" FILES ${RENDERER_FILES})
source_group("Logging" FILES ${LOGGING_FILES})
source_group("Shaders" FILES ${STATIC_SHADERS})

add_library(glframework STATIC ${GL_FILES} ${IMGUI_FILES} ${IMGUI_3D_FILES} ${RENDER_2D_FILES})
target_link_libraries(glframework PUBLIC ${OPENGL_gl_LIBRARY} glfw glad::glad glm)
target_compile_definitions(glframework PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
if(${WITH_EGL})
	target_compile_definitions(glframework PUBLIC -DWITH_EGL)
endif()
if(${WITH_OPENMESH})
	target_link_libraries(glframework PUBLIC OpenMeshCore)
	target_compile_definitions(glframework PUBLIC -D_USE_MATH_DEFINES -DWITH_OPENMESH)
#	target_sources(glframework PRIVATE src/openmesh_ext.h ${OPENMESH_EXT)
	target_sources(glframework PRIVATE
		${INCLUDE_DIR}/meshes/openmesh_mesh.hpp
		${INCLUDE_DIR}/openmesh_ext.hpp
		src/meshes/openmesh_mesh.cpp)
endif()
if(${WITH_ASSIMP})
	target_link_libraries(glframework PUBLIC ${ASSIMP_LIBRARIES})
	target_link_directories(glframework PUBLIC ${ASSIMP_INCLUDE_DIRECTORIES})
	target_link_directories(glframework PUBLIC ${ASSIMP_LIBRARY_DIRS})
	target_compile_definitions(glframework PUBLIC -DWITH_ASSIMP -D_USE_MATH_DEFINES)
endif()

target_compile_definitions(glframework PUBLIC -DGL_FRAMEWORK_SHADER_DIR="${PROJECT_SOURCE_DIR}/shaders/")
target_compile_definitions(glframework PUBLIC -DGL_FRAMEWORK_FONT_DIR="${PROJECT_SOURCE_DIR}/fonts/")
target_compile_definitions(glframework PUBLIC -DGLM_ENABLE_EXPERIMENTAL)
target_compile_features(glframework PRIVATE cxx_std_17)
	
target_include_directories(glframework PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${IMGUI_DIR}>
	$<BUILD_INTERFACE:${IMGUI_PLOT_DIR}>
	$<BUILD_INTERFACE:${IMGUI_DIR}/backends>
	${OPENGL_INCLUDE_DIRS} 
	${GLM_INCLUDE_DIRS})

if(${BUILD_FRAMEWORK_SAMPLES})
	add_subdirectory(samples/)
endif()
