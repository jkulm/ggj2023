# ggj2023
Global Game Jam 2023

Unreal Engine 5.1

FMod<br>
Fmod is a pain because you have to recompile from C++ but that's in and seems to be hooked up properly.
I have the project shoved in source art. Make sure you're using FMOD Studio 2.02.11

\ggj2023\SourceArt<br>
Source Art is where we can shove working files and art files. So there can be a Meshes folder with other folders for fbx and blender files or whatever. Same for Textures and whatever else. This allows us to keep the file pathing relative to the project, so if somebody updates a mesh on a computer other than the one it was created on, reimporting it in engine is a single button click. Otherwise it will be looking for the files in whatever specific drive and folder the artist imported it from.
