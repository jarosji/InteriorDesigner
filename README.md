How to run:

Before building the Unreal Engine project, Assimp library needs to be build first.
1. Open PowerShell in folder Plugins/JJFileImporter/Source/ThirdParty/AssimpLibrary/assimp/
2. Run the following commands:

                                 - cmake CMakeLists.txt
                                 - cmake --build . --config Release
4. In the main folder, generate Visual Studio files from InteriorDesigner.uproject
5. Build project from preffered IDE
