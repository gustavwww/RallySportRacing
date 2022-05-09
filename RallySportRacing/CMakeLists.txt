# CMakeList.txt : CMake project for RallySportRacing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)



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
    "Rendering/Model.h" "Rendering/Model.cpp" "Game/GameManager.h" "Game/GameManager.cpp" "Game/GameObject.h" "Game/GameObject.cpp" "Rendering/Mesh.h" "Rendering/Mesh.cpp" "Rendering/SubMesh.h" "Rendering/SubMesh.cpp" "Physics/Physics.cpp" "Physics/Physics.hpp" "Game/Vehicle.cpp" "Game/Vehicle.h"    "Debugging/DebugDraw.h"   "Debugging/DebugDraw.cpp" "Services/Protocol/ProtocolParser.h" "Services/Protocol/ProtocolParser.cpp" "Services/Protocol/Command.h" "Services/Protocol/Command.cpp" "Services/TCPClient.h" "Services/TCPClient.cpp" "Networking/Networking.h" "Networking/Networking.cpp" "Networking/Player.h" "Networking/Player.cpp" "Services/UDPClient.h" "Services/UDPClient.cpp" "Rendering/ParticleSystem.h"
    "Rendering/ParticleSystem.cpp" "Utils/Random.h" "Utils/Random.cpp" "Utils/GameTimer.h" "Utils/GameTimer.cpp" "Audio/audio.cpp" "Audio/audio.h" "Audio/SoundSource.cpp" "Audio/SoundSource.h" "Networking/PlayerData.h" "Networking/PlayerData.cpp" "Utils/HdrFileGenerator.h" "Utils/HdrFileGenerator.cpp" "Rendering/FrameBufferObject.h" "Rendering/FrameBufferObject.cpp")


find_package(OpenGL REQUIRED)

set(Bullet_ROOT "${CMAKE_SOURCE_DIR}/External/bullet/")
find_package(Bullet REQUIRED)

target_include_directories(RallySportRacing PUBLIC
                            "${CMAKE_BINARY_DIR}"
                            "${OpenGL_INCLUDE_DIR}"
                            "${CMAKE_SOURCE_DIR}/External/GLEW/include"
                            "${CMAKE_SOURCE_DIR}/External/SDL/include"
                            "${CMAKE_SOURCE_DIR}/External/glm"
                            "${CMAKE_SOURCE_DIR}/External/tinygltf"
                            ${BULLET_INCLUDE_DIR}
                            "External/bullet/include/bullet/" 
                          )

target_link_libraries(RallySportRacing PUBLIC 
                            "${CMAKE_SOURCE_DIR}/External/GLEW/lib/Release/x64/glew32.lib"
                            "${CMAKE_SOURCE_DIR}/External/SDL/lib/x64/SDL2.lib"
                            "${CMAKE_SOURCE_DIR}/External/SDL/lib/x64/SDL2main.lib"

                            "${OPENGL_LIBRARIES}"
                            ${BULLET_LIBRARIES}
                            imgui
)

add_custom_command(TARGET RallySportRacing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/External/bin"
        $<TARGET_FILE_DIR:RallySportRacing>)