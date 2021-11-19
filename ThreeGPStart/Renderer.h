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

	// Vertex Array Object to wrap all render settings
	GLuint m_JeepVAO{ 0 };

	GLuint m_SkyBoxVAO{ 0 };

	// Number of elments to use when rendering
	GLuint m_NumJeepElements{ 0 };

	GLuint m_NumSkyBoxElements{ 0 };

	bool m_wireframe{ false };

	bool CreateProgram();

	GLuint Tex;

	GLuint CubeMap;
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

