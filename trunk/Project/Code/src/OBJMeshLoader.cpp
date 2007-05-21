#include <string>
#include <iostream>
#include <fstream>

#include "Vector2.h"
#include "Vector3.h"
#include "Vertex.h"
#include "Mesh.h"

#include "glm_avl.h"
#include "OBJMeshLoader.h"


OBJLoader::OBJLoader(void)
{
}

OBJLoader::~OBJLoader(void)
{
}
/**
* @param string aObjectFileName
* @param vector<string>& aFileNames
* @return 
*/
void OBJLoader::loadModelFileNames( std::string aObjectFileName,
								    std::vector< std::string >& aFileNames, 
									std::string aFilePrefix )
{
	// try to open the object file name
		ifstream input;
		input.open(aObjectFileName.c_str(), ifstream::in);
		
		// if the object file name is open
		if (input.is_open())
		{
			// while input is good
			while (input.good())
			{
				// get a complete line
				std::string str;
				std::getline( input, str );

				str = aFilePrefix + str;

				// open the filename specified on that line
				ifstream objectFile;
				objectFile.open( str.c_str(), ifstream::in);

				// if the objectfile exists
				if ( objectFile.is_open() )
				{
					// close it and store the string in the list
					objectFile.close();
					aFileNames.push_back( str );
				}
			}

			// only need to close the file if it was opened correctly
			input.close();
		}

}

/**
 * @param filename
 * @return 
 */
Mesh* OBJLoader::loadModel(std::string filename)
{
	char filenameChar[255];
	strcpy_s(filenameChar, filename.c_str());
	
	GLMmodel* model = glmReadOBJ( filenameChar );
	glmUnitize( model );
	glmFacetNormals( model );
	glmVertexNormals(model, 60.0f);

	Mesh* mesh = new Mesh();

	GLuint* vIndices;
	GLuint* tIndices;
	GLuint* nIndices;

	// just doing it for one group...
	bool textured = false;
	Vector2<float> texCoords;

	if (model->nummaterials != 0)
	{
		GLuint material = model->groups[0].material;
		textured = model->materials[material].textured;
	}
	
	mesh->setTextured( textured );

	// loop all the triangles
	for (int i = 0; i < model->numtriangles; i++)
	{
		GLMtriangle tri = model->triangles[i];

		// get the indices for Vertices, Texture Coords and Normals
		vIndices = tri.vindices;
		tIndices = tri.tindices;
		nIndices = tri.nindices;
		
		// loop and insert new vertices
		for (int j = 0; j < 3; j++)
		{
			GLuint vI = vIndices[j] * 3;
			GLuint tI = tIndices[j] * 2;
			GLuint nI = nIndices[j] * 3;

			Vector3<float> position(model->vertices[vI], 
				model->vertices[vI+1], model->vertices[vI+2]);

			if (textured)
			{
				texCoords.set(model->texcoords[tI], 
					model->texcoords[tI+1]);
			}

			Vector3<float> normal(model->normals[nI], 
				model->normals[nI+1], model->normals[nI+2]);

			normal.Normalize();

			mesh->addVertex( Vertex(normal, position, texCoords) );
		}
		
		int index = i*3;
		mesh->addTriangleIndices(index, index+1, index+2);
	}

	return mesh;
}