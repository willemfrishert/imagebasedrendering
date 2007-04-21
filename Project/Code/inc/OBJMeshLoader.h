/**
 * OBJLoader: Alias Wavefront OBJ Loader. Running over
 * Nate robins GLM_AVL great obj loader. Very slow to load
 * 
 * @file OBJMeshLoader.h
 * @author João Pedro Jorge
 */
#pragma once

class Mesh;

class OBJLoader
{
public:
	OBJLoader(void);
	~OBJLoader(void);

	static Mesh* loadModel(std::string filename);
};
