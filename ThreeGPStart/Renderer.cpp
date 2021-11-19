#include "Renderer.h"
#include "Camera.h"
#include "ImageLoader.h"

Renderer::Renderer() 
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(m_program);
	glDeleteBuffers(1, &m_JeepVAO);
	glDeleteBuffers(1, &m_SkyBoxVAO);
}

// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("3GP");						// Create a window called "3GP" and append into it.

	ImGui::Text("Visibility.");					// Display some text (you can use a format strings too)	

	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 100000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
	ImGui::End();
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.vert") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.frag") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return true;
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	std::vector<std::string> Faces
	{
		"Data\\Models\\Sky\\Mars\\Mar_R.dds",
		"Data\\Models\\Sky\\Mars\\Mar_L.dds",
		"Data\\Models\\Sky\\Mars\\Mar_U.dds",
		"Data\\Models\\Sky\\Mars\\Mar_D.dds",
		"Data\\Models\\Sky\\Mars\\Mar_F.dds",
		"Data\\Models\\Sky\\Mars\\Mar_B.dds"
	};
	
	Helpers::ImageLoader SkyBoxTexLoader;
	glGenTextures(1, &CubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
	for (int i = 0; i < Faces.size(); i++)
	{

		if (SkyBoxTexLoader.Load(Faces[i]))
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, SkyBoxTexLoader.Width(), SkyBoxTexLoader.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, SkyBoxTexLoader.GetData()
			);
		}
		else 
		{
			MessageBox(NULL, L"Missing a sky box face.", L"Error",
				MB_OK | MB_ICONEXCLAMATION);

			return false;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);




	Helpers::ModelLoader SkyBoxLoader;
	if (!SkyBoxLoader.LoadFromFile("Data\\Models\\Sky\\Mars\\skybox.X"))
	{
		MessageBox(NULL, L"Sky box not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	for (const Helpers::Mesh& mesh : SkyBoxLoader.GetMeshVector())
	{
		GLuint PositionsSkyBoxVBO;
		glGenBuffers(1, &PositionsSkyBoxVBO);

		glBindBuffer(GL_ARRAY_BUFFER, PositionsSkyBoxVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		GLuint SkyBoxCoordsVBO;
		glGenBuffers(1, &SkyBoxCoordsVBO);

		glBindBuffer(GL_ARRAY_BUFFER, SkyBoxCoordsVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		GLuint ElementSkyBoxEBO;
		glGenBuffers(1, &ElementSkyBoxEBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementSkyBoxEBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_NumSkyBoxElements = mesh.elements.size();
		// TODO: create a VBA to wrap everything and specify locations in the shaders
		glGenVertexArrays(1, &m_SkyBoxVAO);


		glBindVertexArray(m_SkyBoxVAO);

		glBindBuffer(GL_ARRAY_BUFFER, PositionsSkyBoxVBO);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementSkyBoxEBO);

		glBindVertexArray(0);

		glBindVertexArray(m_SkyBoxVAO);

		glBindBuffer(GL_ARRAY_BUFFER, SkyBoxCoordsVBO);

		glEnableVertexAttribArray(1);

		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

	}
	
	// Helpers has an object for loading 3D geometry, supports most types

	
// Load in the jeep
	Helpers::ModelLoader JeepLoader;
	if (!JeepLoader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
	{
		MessageBox(NULL, L"Jeep not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}

	Helpers::ImageLoader JeepTexLoader;
	if (JeepTexLoader.Load("Data\\Models\\Jeep\\jeep_army.jpg"))
	{
		glGenTextures(1, &Tex);
		glBindTexture(GL_TEXTURE_2D, Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, JeepTexLoader.Width(), JeepTexLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, JeepTexLoader.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{
		MessageBox(NULL, L"Jeep texture not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	// Now we can loop through all the mesh in the loaded model:
	for (const Helpers::Mesh& mesh : JeepLoader.GetMeshVector())
	{
		// We can extract from the mesh via:
		//mesh.vertices  - a vector of glm::vec3 (3 floats) giving the position of each vertex
		//mesh.elements - a vector of unsigned int defining which vert make up each triangle
		// TODO: create VBO for the vertices and a EBO for the elements
		GLuint PositionsJeepVBO;
		glGenBuffers(1, &PositionsJeepVBO);

		glBindBuffer(GL_ARRAY_BUFFER, PositionsJeepVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		GLuint CoordsVBO;
		glGenBuffers(1, &CoordsVBO);

		glBindBuffer(GL_ARRAY_BUFFER, CoordsVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		GLuint ElementJeepEBO;
		glGenBuffers(1, &ElementJeepEBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementJeepEBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_NumJeepElements = mesh.elements.size();
		// TODO: create a VBA to wrap everything and specify locations in the shaders
		glGenVertexArrays(1, &m_JeepVAO);


		glBindVertexArray(m_JeepVAO);

		glBindBuffer(GL_ARRAY_BUFFER, PositionsJeepVBO);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementJeepEBO);

		glBindVertexArray(0);

		glBindVertexArray(m_JeepVAO);

		glBindBuffer(GL_ARRAY_BUFFER, CoordsVBO);

		glEnableVertexAttribArray(1);

		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		

	}

	

	


	/*
		TODO 3: You also need to create an element buffer
		Store the number of elements in the member variable m_numElements
	*/

	

	/*
		TODO 4: Finally create a VAO to wrap the buffers. You need to specify the streams for the positions
		(attribute 0) and colours (attribute 1). You also need to bind the element buffer.
		Use the member variable m_VAO
	*/

	

	/*
		TODO 5: Run it and see if you can see the cube.
		You should not have to edit the render function or shaders to see the cube.
	*/

	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{			
	// Configure pipeline settings
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 1500.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::mat4(glm::mat3(view_xform));
	glm::mat4 combined_xform = projection_xform * view_xform;
	

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);

	// Send the combined matrix to the shader in a uniform
	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	glm::mat4 model_xform = glm::mat4(1);

	// Send the model matrix to the shader in a uniform
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Tex);
	glUniform1i(glGetUniformLocation(m_program, "SamplerTex"), 0);
	
	glDepthMask(GL_FALSE);
	glBindVertexArray(m_SkyBoxVAO);
	glDrawElements(GL_TRIANGLES, m_NumSkyBoxElements, GL_UNSIGNED_INT, (void*)0);
	//glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_INT, (void*)0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glm::mat4 view_xform2 = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	combined_xform = projection_xform * view_xform2;
	// Bind our VAO and render
	glBindVertexArray(m_JeepVAO);
	glDrawElements(GL_TRIANGLES, m_NumJeepElements, GL_UNSIGNED_INT, (void*)0);

}

