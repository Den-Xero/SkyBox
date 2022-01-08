#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	GLuint SkyBox_program{ 0 };

	GLuint TerrainProgram{ 0 };

	GLuint CubeProgram{ 0 };

	// Vertex Array Object to wrap all render settings
	GLuint m_JeepVAO{ 0 };

	GLuint m_SkyBoxVAO{ 0 };

	GLuint m_CubeVAO{ 0 };

	GLuint m_TerrainVAO{ 0 };

	// Number of elments to use when rendering
	GLuint m_NumJeepElements{ 0 };

	GLuint m_numCubeElements{ 0 };

	GLuint m_numTerrainElements{ 0 };

	bool m_wireframe{ false };

	bool CreateJeepProgram();

	bool CreateSkyProgram();

	bool CreateTerrainProgram();

	bool CreateCubeProgram();

	bool Swap{ true };

	bool NoiseOn{ true };

	bool ExtraNoise{ true };

	float NoiseValue{ 0 };

	size_t Index{ 0 };

	GLuint TerrainTex{ 0 };

	GLuint HightmapTex{ 0 };

	GLuint JeepTex{ 0 };

	GLuint CubeMap{ 0 };
public:
	Renderer();
	~Renderer();

	// Draw GUI
	void DefineGUI();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

