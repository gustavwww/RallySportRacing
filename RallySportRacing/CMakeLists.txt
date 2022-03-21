# CMakeList.txt : CMake project for RallySportRacing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)



file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.frag"
)

source_group("Shaders" FILES ${SHADERS})

# Add source to this project's executable.
add_executable (RallySportRacing
    "main.cpp"
    "Rendering/SDLWindowHandler.cpp"
    "Rendering/SDLWindowHandler.h"
    ${SHADERS}
    "Rendering/Model.h" "Rendering/Model.cpp"   "Game/GameManager.h" "Game/GameManager.cpp" "Game/GameObject.h" "Game/GameObject.cpp" "Rendering/Mesh.h" "Rendering/Mesh.cpp" "Rendering/SubMesh.h" "Rendering/SubMesh.cpp" "Physics/Physics.cpp" "Physics/Physics.hpp")

find_package(OpenGL REQUIRED)

target_include_directories(RallySportRacing PUBLIC
                            "${CMAKE_BINARY_DIR}"
                            "${OpenGL_INCLUDE_DIR}"
                            "${CMAKE_SOURCE_DIR}/External/GLEW/include"
                            "${CMAKE_SOURCE_DIR}/External/SDL/include"
                            "${CMAKE_SOURCE_DIR}/External/glm"
                            "${CMAKE_SOURCE_DIR}/External/tinygltf"
                            "${CMAKE_SOURCE_DIR}/External/bullet"
                          )

target_link_libraries(RallySportRacing PUBLIC 
                            "${CMAKE_SOURCE_DIR}/External/GLEW/lib/Release/x64/glew32.lib"
                            "${CMAKE_SOURCE_DIR}/External/SDL/lib/x64/SDL2.lib"
                            "${CMAKE_SOURCE_DIR}/External/SDL/lib/x64/SDL2main.lib"

                            "${CMAKE_SOURCE_DIR}/External/bullet/Bullet2FileLoader.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/Bullet3Collision.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/Bullet3Common.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/Bullet3Dynamics.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/Bullet3Geometry.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/Bullet3OpenCL_clew.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/BulletCollision.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/BulletDynamics.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/BulletInverseDynamics.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/BulletSoftBody.lib"
                            "${CMAKE_SOURCE_DIR}/External/bullet/LinearMath.lib"
                            "${OPENGL_LIBRARIES}"
                            imgui
)

add_custom_command(TARGET RallySportRacing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/External/bin"
        $<TARGET_FILE_DIR:RallySportRacing>)