#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Material
{
	typedef struct {Texture texture; Color color; float factor;} Component;
	Component diffuse;
	Component specular;
	Component normal;
	Component rough;
	Component emission;
};

struct Vertex
{
	struct {float x, y, z;} position;
	struct {float x, y, z;} normal;
	struct {float u, v;} uv1;
	struct {float u, v;} uv2;
	struct {float r, g, b, a;} color;
};

struct Mesh
{	
	uint vao, vbo, ebo;
	
	uint* indices;
	uint numIndices;
	
	Vertex* vertices;
	uint numVertices;

	int materialId;
	
	void draw(Array<Material> materials)
	{		
		char name[16];
		Shader* shader = activeShader();
		
		Material* material = &materials[materialId];
		
		shader->setTexture("diffuseTex", material->diffuse.texture);
		shader->setTexture("specularTex", material->specular.texture);
		shader->setTexture("normalTex", material->normal.texture);
		shader->setTexture("roughTex", material->rough.texture);
		shader->setTexture("emissionTex", material->emission.texture);
		
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}
};

Mesh createMesh()
{
	Mesh ret;
	
	ret.vao = 0;
	ret.vbo = 0;
	ret.ebo = 0;
	
	ret.indices = 0;
	ret.numIndices = 0;
	
	ret.vertices = 0;
	ret.numVertices = 0;
	
	ret.materialId = -1;
		
	return ret;
}

Mesh createMesh(aiMesh* mesh, const aiScene* scene)
{
	Mesh ret;
	
	// assume three vertices per face
	// for simplicity
	ret.numIndices = mesh->mNumFaces * 3;
	ret.numVertices = mesh->mNumVertices;
	
	ret.indices = (uint*) malloc(ret.numIndices * sizeof(uint));
	ret.vertices = (Vertex*) malloc(ret.numVertices * sizeof(Vertex));

	// collect vertex data
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		// positions
		ret.vertices[i].position.x = mesh->mVertices[i].x;
		ret.vertices[i].position.y = mesh->mVertices[i].y;
		ret.vertices[i].position.z = mesh->mVertices[i].z;
		
		// normals
		if (mesh->HasNormals())
	{
		ret.vertices[i].normal.x = mesh->mNormals[i].x;
		ret.vertices[i].normal.y = mesh->mNormals[i].y;
		ret.vertices[i].normal.z = mesh->mNormals[i].z;
	}
		else
	{
		printf("No normals while loading mesh %p\n", &ret);
		// NOTE: the address here is garbage, since ret is a
		// temporary variable and gets deleted at the end of the function.
		// nevertheless, it can be useful to distinguish, when loading
		// multiple meshes, for debugging purposes
		
		ret.vertices[i].normal = {0,1,0};
	}
		
		// texture coordinates
		// there may be up to 8 different uv sets per vertex
		// we assume only the first set is used
		if (mesh->mTextureCoords[0])
	{
		ret.vertices[i].uv1.u = mesh->mTextureCoords[0][i].x;
		ret.vertices[i].uv1.v = mesh->mTextureCoords[0][i].y;
	}
		// if we didnt find a uv for the vertex,
		// default to (0,0)
		else
	{
		ret.vertices[i].uv1.u = 0.0;
		ret.vertices[i].uv1.v = 0.0;
	}
		// colors
		// only use first set of colors if present
		if (mesh->HasVertexColors(0))
	{
		ret.vertices[i].color.r = mesh->mColors[i]->r;
		ret.vertices[i].color.g = mesh->mColors[i]->g;
		ret.vertices[i].color.b = mesh->mColors[i]->b;
		ret.vertices[i].color.a = mesh->mColors[i]->a;
	}
		// default to white
		else
	{
		ret.vertices[i].color = {255u,255u,255u,255u};
	}
	
	}
	
	// copy indices
	for(uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		
		ret.indices[i * 3 + 0] = face.mIndices[0];
		ret.indices[i * 3 + 1] = face.mIndices[1];
		ret.indices[i * 3 + 2] = face.mIndices[2];
	}
	
	// material index
	ret.materialId = mesh->mMaterialIndex;
		
	return ret;
}

void uploadMesh(Mesh& mesh)
{
	glGenVertexArrays(1, &mesh.vao);
	glGenBuffers(1, &mesh.vbo);
	glGenBuffers(1, &mesh.ebo);

	glBindVertexArray(mesh.vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.numVertices, mesh.vertices, GL_STATIC_DRAW); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.numIndices, mesh.indices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*) offsetof(Vertex, position));
	glEnableVertexAttribArray(0);	
		
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*) offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	// uv1
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*) offsetof(Vertex, uv1));
	glEnableVertexAttribArray(2);	

	// uv2
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*) offsetof(Vertex, uv2));
	glEnableVertexAttribArray(3);

	// color
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*) offsetof(Vertex, color));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
}

// collection of meshes and materials
struct Model
{
	Array<Mesh> meshes;
	Array<Material> materials;
	
	void draw()
	{
		for (int i = 0; i < meshes.size; i++)
		{
			meshes[i].draw(materials);
		}
	}
};

Array<Material> getMaterials(const aiScene* scene)
{
	Array<Material> ret;
	ret.allocate(scene->mNumMaterials);
		
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* aimat = scene->mMaterials[i];
		
		aiColor4D color;
		
		aimat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		ret[i].diffuse.color = {color.r, color.g, color.b, color.a};
		
		aimat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		ret[i].specular.color = {color.r, color.g, color.b, color.a};
		
		aimat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		ret[i].emission.color = {color.r, color.g, color.b, color.a};
					
		ret[i].normal.color = {1,1,1,1};
		ret[i].rough.color = {1,1,1,1};
		
		ai_real factor;
		
		aimat->Get(AI_MATKEY_SHININESS, factor);
		ret[i].specular.factor = factor;
		
		aimat->Get(AI_MATKEY_REFLECTIVITY, factor);
		ret[i].rough.factor = 1.0 - factor;
					
		ret[i].diffuse.factor = 1.0;
		ret[i].normal.factor = 1.0;
		ret[i].emission.factor = 1.0;
		
		aiTextureType types[] = 
		{
			aiTextureType_DIFFUSE,
			aiTextureType_SPECULAR,
			aiTextureType_NORMALS,
			aiTextureType_DIFFUSE_ROUGHNESS,
			aiTextureType_EMISSION_COLOR,
		};
		
		for (int type = 0; type < 5; type++)
		{
			int textureCount = aimat->GetTextureCount(types[type]);
			
			if (textureCount < 1)
				continue;
			
			aiString path;
			aimat->GetTexture(types[type], 0, &path);
			
			// CAREFUL! number and order of components
			// must match material struct
			Texture* texture = (Texture*)
				&( (Material::Component*)&ret[i].diffuse.texture )[type];
			*texture = createTexture(createImage(path.C_Str()));
		}
	}
	
	return ret;
}

void getMeshesRecursive(aiNode* node, const aiScene* scene, Array<Mesh>& meshes)
{	
	// process each mesh located at the current node
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
		// load mesh struct and append to array
		Mesh m = createMesh(mesh, scene);
		uploadMesh(m);
		meshes.append(m);
	}
	
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		getMeshesRecursive(node->mChildren[i], scene, meshes);
	}
}

Array<Mesh> getMeshes(const aiScene* scene)
{
	Array<Mesh> ret;
	ret.allocate(scene->mNumMeshes);
	// there may be more meshes down the tree,
	// but this is a good starting point
	
	getMeshesRecursive(scene->mRootNode, scene, ret);
	// the "bootstrap"; calling the actual recursive function
	
	ret.shrink(); // get rid of extra allocated memory
	
	return ret;
}

Model createModel(char* filePath, int flipUv = false)
{
	Model ret;
	
	Assimp::Importer importer;
	
	aiPostProcessSteps flags = (aiPostProcessSteps)(0
		| aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace
		| (flipUv) ? aiProcess_FlipUVs : 0
	);
	
	const aiScene* scene = importer.ReadFile(filePath, flags);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		printf("ERROR: Assimp: Import model failed!\n%s\n", importer.GetErrorString());
		return;
	}
	
	ret.materials = getMaterials(scene);
	ret.meshes = getMeshes(scene);
	
	return ret;
}