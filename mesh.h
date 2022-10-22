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
	struct {u8 r, g, b, a;} color;
};

struct Mesh
{	
	uint vao, vbo, ebo;
	
	uint* indices;
	uint numIndices;
	
	Vertex* vertices;
	uint numVertices;

	int materialId;
	
	void init()
	{
		vao = 0;
		vbo = 0;
		ebo = 0;
		
		indices = 0;
		numIndices = 0;
		
		vertices = 0;
		numVertices = 0;
		
		materialId = -1;
	}
	
	void initBuffers(uint _numVertices, uint _numFaces)
	{
		// assume three vertices per face
		// for simplicity
		numIndices = _numFaces * 3;
		numVertices = _numVertices;
		
		indices = (uint*) malloc(numIndices * sizeof(uint));
		
		vertices = (Vertex*) malloc(numVertices * sizeof(Vertex));
	}
	
	void loadAi(aiMesh* mesh, const aiScene* scene)
	{
		initBuffers(mesh->mNumVertices, mesh->mNumFaces);

		// collect vertex data
		for (uint i = 0; i < mesh->mNumVertices; i++)
		{
			// positions
			vertices[i].position.x = mesh->mVertices[i].x;
			vertices[i].position.y = mesh->mVertices[i].y;
			vertices[i].position.z = mesh->mVertices[i].z;
			
			// normals
			if (mesh->HasNormals())
		{
			vertices[i].normal.x = mesh->mNormals[i].x;
			vertices[i].normal.y = mesh->mNormals[i].y;
			vertices[i].normal.z = mesh->mNormals[i].z;
		}
			else
		{
			printf("No normals while loading mesh %p\n", this);
			vertices[i].normal = {0,1,0};
		}
			
			// texture coordinates
			// there may be up to 8 different uv sets per vertex
			// we assume only the first set is used
			if (mesh->mTextureCoords[0])
		{
			vertices[i].uv1.u = mesh->mTextureCoords[0][i].x;
			vertices[i].uv1.v = mesh->mTextureCoords[0][i].y;
		}
			// if we didnt find a uv for the vertex,
			// default to (0,0)
			else
		{
			vertices[i].uv1.u = 0.0;
			vertices[i].uv1.v = 0.0;
		}
			// colors
			// only use first set of colors if present
			if (mesh->HasVertexColors(0))
		{
			vertices[i].color.r = mesh->mColors[i]->r;
			vertices[i].color.g = mesh->mColors[i]->g;
			vertices[i].color.b = mesh->mColors[i]->b;
			vertices[i].color.a = mesh->mColors[i]->a;
		}
			// default to white
			else
		{
			vertices[i].color = {255u,255u,255u,255u};
		}
      
		}
		
		// copy indices
		for(uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			
			indices[i * 3 + 0] = face.mIndices[0];
			indices[i * 3 + 1] = face.mIndices[1];
			indices[i * 3 + 2] = face.mIndices[2];
		}
		
		// material index
		materialId = mesh->mMaterialIndex;
	}
	
	// upload to GPU
	// must be called to use mesh
	void upload()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	
		glBindVertexArray(vao);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW); 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * numIndices, indices, GL_STATIC_DRAW);

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
		glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex),
			(void*) offsetof(Vertex, color));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}
	
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

// collection of meshes and materials
struct Model
{
	Array<Mesh> meshes;
	Array<Material> materials;
	
	void loadMeshes(aiNode* node, const aiScene* scene)
	{
		meshes.allocate(scene->mNumMeshes);
		
		// process each mesh located at the current node
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			
			// load mesh struct and append to array
			Mesh m;
			m.init();
			m.loadAi(mesh, scene);
			m.upload();
			meshes.append(m);
		}
		
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			loadMeshes(node->mChildren[i], scene);
		}
	}
	
	void loadMaterials(const aiScene* scene)
	{
		materials.allocate(scene->mNumMaterials);
		
		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* aimat = scene->mMaterials[i];
			
			aiColor4D color;
			
			aimat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			materials[i].diffuse.color = {color.r, color.g, color.b, color.a};
			
			aimat->Get(AI_MATKEY_COLOR_SPECULAR, color);
			materials[i].specular.color = {color.r, color.g, color.b, color.a};
			
			aimat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			materials[i].emission.color = {color.r, color.g, color.b, color.a};
						
			materials[i].normal.color = {1,1,1,1};
			materials[i].rough.color = {1,1,1,1};
			
			ai_real factor;
			
			aimat->Get(AI_MATKEY_SHININESS, factor);
			materials[i].specular.factor = factor;
			
			aimat->Get(AI_MATKEY_REFLECTIVITY, factor);
			materials[i].rough.factor = 1.0 - factor;
						
			materials[i].diffuse.factor = 1.0;
			materials[i].normal.factor = 1.0;
			materials[i].emission.factor = 1.0;
			
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
					&( (Material::Component*)&materials[i].diffuse.texture )[type];
				Image image;
				
				image.loadFile(path.C_Str());
				texture->loadImage(image);
			}
		}
	}
	
	void loadFile(char* path, u8 flipUv = false)
	{
		Assimp::Importer importer;
		
		aiPostProcessSteps flags = (aiPostProcessSteps)(0
			| aiProcess_Triangulate
			| aiProcess_GenSmoothNormals
			| aiProcess_CalcTangentSpace
			| (flipUv) ? aiProcess_FlipUVs : 0
		);
		
		const aiScene* scene = importer.ReadFile(path, flags);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			printf("ERROR: Assimp: Import model failed!\n%s\n", importer.GetErrorString());
			return;
		}
		
		loadMaterials(scene);
		loadMeshes(scene->mRootNode, scene);
	}
	
	void draw()
	{
		for (int i = 0; i < meshes.size; i++)
		{
			meshes[i].draw(materials);
		}
	}
};
