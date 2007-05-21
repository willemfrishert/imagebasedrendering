/**
 * OBJLoader: Alias Wavefront OBJ Loader. Running over
 * Nate robins GLM_AVL great obj loader. Very slow to load
 * 
 * @file OBJMeshLoader.h
 * @author João Pedro Jorge
 * @co-author Willem Frishert :P
 */
#pragma once

class Mesh;

class OBJLoader
{
public:
	OBJLoader(void);
	~OBJLoader(void);

	static void loadModelFileNames( std::string aObjectFileName, 
									std::vector< std::string >& aFileNames, 
									std::string aFilePrefix );
	static Mesh* loadModel( std::string filename );
};
