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
	glDeleteProgram(SkyBox_program);
	glDeleteBuffers(1, &m_JeepVAO);
	glDeleteBuffers(1, &m_SkyBoxVAO);
	glDeleteProgram(TerrainProgram);
	glDeleteProgram(CubeProgram);
	glDeleteBuffers(1, &m_CubeVAO);
	glDeleteBuffers(1, &m_TerrainVAO);
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
bool Renderer::CreateJeepProgram()
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

bool Renderer::CreateSkyProgram()
{
	// Create a new program (returns a unqiue id)
	SkyBox_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint SkyBox_vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/SkyBox_vertex_shader.vert") };
	GLuint SkyBox_fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/SkyBox_fragment_shader.frag") };
	if (SkyBox_vertex_shader == 0 || SkyBox_fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(SkyBox_program, SkyBox_vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(SkyBox_program, SkyBox_fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(SkyBox_vertex_shader);
	glDeleteShader(SkyBox_fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(SkyBox_program))
		return false;

	return true;
}

bool Renderer::CreateTerrainProgram()
{
	// Create a new program (returns a unqiue id)
	TerrainProgram = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint Terrain_vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/Terrain_vertex_shader.vert") };
	GLuint Terrain_fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/Terrain_fragment_shader.frag") };
	if (Terrain_vertex_shader == 0 || Terrain_fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(TerrainProgram, Terrain_vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(TerrainProgram, Terrain_fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(Terrain_vertex_shader);
	glDeleteShader(Terrain_fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(TerrainProgram))
		return false;

	return true;
}

bool Renderer::CreateCubeProgram()
{
	// Create a new program (returns a unqiue id)
	CubeProgram = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint Cube_vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/Cube_vertex_shader.vert") };
	GLuint Cube_fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/Cube_fragment_shader.frag") };
	if (Cube_vertex_shader == 0 || Cube_fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(CubeProgram, Cube_vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(CubeProgram, Cube_fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(Cube_vertex_shader);
	glDeleteShader(Cube_fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(CubeProgram))
		return false;

	return true;
}


float Noise(int X, int Y)
{
	//makes noise to be used on the terrain.
	int N = X + Y * 24;
	N = (N >> 13) ^ N;
	int NN = (N * (N * N * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)NN / 1073741824.0f);
}


// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateJeepProgram())
		return false;
	if (!CreateSkyProgram())
		return false;
	if (!CreateTerrainProgram())
		return false;
	if (!CreateCubeProgram())
		return false;

	//skybox Positions
	std::vector<GLfloat> SkyBoxVertices = {
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f
	};
	
	//image loaders for the six sides of the cube.
	Helpers::ImageLoader SkyBoxTexLoaderRight;
	Helpers::ImageLoader SkyBoxTexLoaderLeft;
	Helpers::ImageLoader SkyBoxTexLoaderUp;
	Helpers::ImageLoader SkyBoxTexLoaderDown;
	Helpers::ImageLoader SkyBoxTexLoaderFront;
	Helpers::ImageLoader SkyBoxTexLoaderBack;
	//loads the image on to the image loaders.
	if (!SkyBoxTexLoaderRight.Load("Data\\Models\\Sky\\Mars\\Mar_R.dds"))
	{
		MessageBox(NULL, L"Right not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	else if (!SkyBoxTexLoaderLeft.Load("Data\\Models\\Sky\\Mars\\Mar_L.dds"))
	{
		MessageBox(NULL, L"Left not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	else if (!SkyBoxTexLoaderUp.Load("Data\\Models\\Sky\\Mars\\Mar_U.dds"))
	{
		MessageBox(NULL, L"Up not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	else if (!SkyBoxTexLoaderDown.Load("Data\\Models\\Sky\\Mars\\Mar_D.dds"))
	{
		MessageBox(NULL, L"Down not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	else if (!SkyBoxTexLoaderFront.Load("Data\\Models\\Sky\\Mars\\Mar_F.dds"))
	{
		MessageBox(NULL, L"front not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	else if (!SkyBoxTexLoaderBack.Load("Data\\Models\\Sky\\Mars\\Mar_B.dds"))
	{
		MessageBox(NULL, L"Back not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	//makes the cube map with the textures on each side.
	glGenTextures(1, &CubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, SkyBoxTexLoaderRight.Width(), SkyBoxTexLoaderRight.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, SkyBoxTexLoaderRight.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, SkyBoxTexLoaderLeft.Width(), SkyBoxTexLoaderLeft.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, SkyBoxTexLoaderLeft.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, SkyBoxTexLoaderUp.Width(), SkyBoxTexLoaderUp.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, SkyBoxTexLoaderUp.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, SkyBoxTexLoaderDown.Width(), SkyBoxTexLoaderDown.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, SkyBoxTexLoaderDown.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, SkyBoxTexLoaderFront.Width(), SkyBoxTexLoaderFront.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, SkyBoxTexLoaderFront.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, SkyBoxTexLoaderBack.Width(), SkyBoxTexLoaderBack.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, SkyBoxTexLoaderBack.GetData());
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	//loads the Positions to be warpped
	GLuint SkyBoxCoordsVBO;
	glGenBuffers(1, &SkyBoxCoordsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, SkyBoxCoordsVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * SkyBoxVertices.size(), SkyBoxVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//warps the skybox.
	glGenVertexArrays(1, &m_SkyBoxVAO);

	glBindVertexArray(m_SkyBoxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, SkyBoxCoordsVBO);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindVertexArray(0);

	

	
// Load in the jeep
	Helpers::ModelLoader JeepLoader;
	if (!JeepLoader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
	{
		MessageBox(NULL, L"Jeep not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	//loads jeep texture
	Helpers::ImageLoader JeepTexLoader;
	if (JeepTexLoader.Load("Data\\Models\\Jeep\\jeep_army.jpg"))
	{
		glGenTextures(1, &JeepTex);
		glBindTexture(GL_TEXTURE_2D, JeepTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, JeepTexLoader.Width(), JeepTexLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, JeepTexLoader.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);
		/*glBindTexture(GL_TEXTURE_2D, 0);*/

	}
	else
	{
		MessageBox(NULL, L"Jeep texture not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	//loops through all the mesh in the model loader:
	for (const Helpers::Mesh& mesh : JeepLoader.GetMeshVector())
	{
		//loads the Positions to be warpped
		GLuint PositionsJeepVBO;
		glGenBuffers(1, &PositionsJeepVBO);

		glBindBuffer(GL_ARRAY_BUFFER, PositionsJeepVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//loads the UVcoords to be warpped
		GLuint CoordsJeepVBO;
		glGenBuffers(1, &CoordsJeepVBO);

		glBindBuffer(GL_ARRAY_BUFFER, CoordsJeepVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//loads the elements to be warpped
		GLuint ElementJeepEBO;
		glGenBuffers(1, &ElementJeepEBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementJeepEBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//sets the number of point that will need to be drawn
		m_NumJeepElements = mesh.elements.size();

		//warps jeep
		glGenVertexArrays(1, &m_JeepVAO);

		//warps positions
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
		//warps elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementJeepEBO);

		glBindVertexArray(0);
		//warps UVcoords
		glBindVertexArray(m_JeepVAO);

		glBindBuffer(GL_ARRAY_BUFFER, CoordsJeepVBO);

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

	glBindVertexArray(0);
	
	//cube Positions
	glm::vec3 FrontCubeCorners[4] =
	{

		{-100, -100, 100}, //1
		{100, -100, 100 }, //2
		{-100, 100, 100}, //3
		{100, 100, 100} //4

	};

	glm::vec3 RightCubeCorners[4] =
	{

		{100, -100, 100 }, //1
		{100, -100, -100 }, //2
		{100, 100, 100}, //3
		{100, 100, -100} //4

	};

	glm::vec3 LeftCubeCorners[4] =
	{

		{-100, -100, 100 }, //1
		{-100, -100, -100 }, //2
		{-100, 100, 100}, //3
		{-100, 100, -100} //4

	};

	glm::vec3 BackCubeCorners[4] =
	{

		{-100, -100, -100}, //1
		{100, -100, -100 }, //2
		{-100, 100, -100}, //3
		{100, 100, -100} //4

	};

	glm::vec3 TopCubeCorners[4] =
	{

		{-100, 100, 100}, //1
		{100, 100, 100}, //2
		{-100, 100, -100}, //3
		{100, 100, -100} //4

	};

	glm::vec3 BottomCubeCorners[4] =
	{

		{-100, -100, 100}, //1
		{100, -100, 100 }, //2
		{-100, -100, -100}, //3
		{100, -100, -100 } //4

	};

	//cube colours
	std::vector<GLfloat> Colours =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f

	};
	//puts cube corners in order to be drawn
	std::vector<glm::vec3> Verts;
	Verts.push_back(FrontCubeCorners[0]);//0
	Verts.push_back(FrontCubeCorners[1]);//1
	Verts.push_back(FrontCubeCorners[2]);//2
	Verts.push_back(FrontCubeCorners[3]);//3
	Verts.push_back(RightCubeCorners[0]);//4
	Verts.push_back(RightCubeCorners[1]);//5
	Verts.push_back(RightCubeCorners[2]);//6
	Verts.push_back(RightCubeCorners[3]);//7
	Verts.push_back(LeftCubeCorners[0]);//8
	Verts.push_back(LeftCubeCorners[1]);//9
	Verts.push_back(LeftCubeCorners[2]);//10
	Verts.push_back(LeftCubeCorners[3]);//11
	Verts.push_back(BackCubeCorners[0]);//12
	Verts.push_back(BackCubeCorners[1]);//13
	Verts.push_back(BackCubeCorners[2]);//14
	Verts.push_back(BackCubeCorners[3]);//15
	Verts.push_back(TopCubeCorners[0]);//16
	Verts.push_back(TopCubeCorners[1]);//17
	Verts.push_back(TopCubeCorners[2]);//18
	Verts.push_back(TopCubeCorners[3]);//19
	Verts.push_back(BottomCubeCorners[0]);//20
	Verts.push_back(BottomCubeCorners[1]);//21
	Verts.push_back(BottomCubeCorners[2]);//22
	Verts.push_back(BottomCubeCorners[3]);//23


	//puts the corners into the element
	std::vector<GLuint> Elements;
	Elements.push_back(0);
	Elements.push_back(1);
	Elements.push_back(2);

	Elements.push_back(1);
	Elements.push_back(3);
	Elements.push_back(2);

	Elements.push_back(4);
	Elements.push_back(5);
	Elements.push_back(6);

	Elements.push_back(5);
	Elements.push_back(7);
	Elements.push_back(6);

	Elements.push_back(9);
	Elements.push_back(8);
	Elements.push_back(10);

	Elements.push_back(9);
	Elements.push_back(10);
	Elements.push_back(11);

	Elements.push_back(12);
	Elements.push_back(14);
	Elements.push_back(13);

	Elements.push_back(13);
	Elements.push_back(14);
	Elements.push_back(15);

	Elements.push_back(16);
	Elements.push_back(17);
	Elements.push_back(18);

	Elements.push_back(17);
	Elements.push_back(19);
	Elements.push_back(18);

	Elements.push_back(20);
	Elements.push_back(22);
	Elements.push_back(21);

	Elements.push_back(21);
	Elements.push_back(22);
	Elements.push_back(23);
	/*
		TODO 2: Next you need to create VBOs for the vertices and the colours
		You can look back to last week for examples
	*/

	//loads the Positions to be warpped
	GLuint PositionsVBO;
	glGenBuffers(1, &PositionsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, PositionsVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Verts.size(), Verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//loads the colour to be warpped
	GLuint ColourVBO;
	glGenBuffers(1, &ColourVBO);

	glBindBuffer(GL_ARRAY_BUFFER, ColourVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * Colours.size(), Colours.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//loads the elements to be warpped
	GLuint ElementEBO;
	glGenBuffers(1, &ElementEBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementEBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * Elements.size(), Elements.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_numCubeElements = Elements.size();

	
	//wraps cube
	glGenVertexArrays(1, &m_CubeVAO);

	//warps cube position
	glBindVertexArray(m_CubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, PositionsVBO);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	//warps cube elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementEBO);

	glBindVertexArray(0);


	//warps cube colour
	glBindVertexArray(m_CubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, ColourVBO);

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);


	glBindVertexArray(0);
	



	std::vector<glm::vec3> TerrainVerts;

	std::vector<glm::vec3> Normals;

	std::vector<glm::vec2> UVCoords;

	std::vector<GLuint> TerrainElements;


	float NumCellX{ 400 };
	float NumCellZ{ 400 };

	int NumVertX = NumCellX + 1;
	int NumVertZ = NumCellZ + 1;

	//terrains hightmap
	Helpers::ImageLoader HightmapTexLoader;
	if (HightmapTexLoader.Load("Data\\Heightmaps\\UE4.jpg"))
	{
		float VertexXtoImage = ((float)HightmapTexLoader.Width() - 1) / NumVertX;
		float VertexZtoImage = ((float)HightmapTexLoader.Height() - 1) / NumVertZ;

		BYTE* ImageData = HightmapTexLoader.GetData();

		for (size_t i = 0; i < NumVertZ; i++)
		{
			for (size_t j = 0; j < NumVertX; j++)
			{
				float ImageX = VertexXtoImage * (NumVertX - i);
				float ImageZ = VertexZtoImage * j;

				size_t Offset = ((size_t)ImageX + (size_t)ImageZ * HightmapTexLoader.Width()) * 4;
				BYTE Hight = ImageData[Offset];

				TerrainVerts.push_back(glm::vec3(i * 8, (float)Hight, j * 8));
				Normals.push_back(glm::vec3(0, 1, 0));
				UVCoords.push_back(glm::vec2(j / NumCellX, i / NumCellZ));
			}
		}

	}
	else
	{
		for (size_t i = 0; i < NumVertZ; i++)
		{
			for (size_t j = 0; j < NumVertX; j++)
			{
				TerrainVerts.push_back(glm::vec3(i * 8, 0, j * 8));
				Normals.push_back(glm::vec3(0, 1, 0));
				UVCoords.push_back(glm::vec2(j / NumCellX, i / NumCellZ));
			}
		}
	}

	//places terrain in diamond pattern
	for (size_t CellZ = 0; CellZ < NumCellZ; CellZ++)
	{
		for (size_t CellX = 0; CellX < NumCellX; CellX++)
		{
			int StartVert = (CellZ * NumVertX) + CellX;
			if (Swap)
			{
				TerrainElements.push_back(StartVert);
				TerrainElements.push_back(StartVert + 1);
				TerrainElements.push_back(StartVert + NumVertX);

				TerrainElements.push_back(StartVert + 1);
				TerrainElements.push_back(StartVert + NumVertX + 1);
				TerrainElements.push_back(StartVert + NumVertX);

			}
			else
			{
				TerrainElements.push_back(StartVert);
				TerrainElements.push_back(StartVert + NumVertX + 1);
				TerrainElements.push_back(StartVert + NumVertX);

				TerrainElements.push_back(StartVert + 1);
				TerrainElements.push_back(StartVert + NumVertX + 1);
				TerrainElements.push_back(StartVert);

			}

			Swap = !Swap;
		}

		Swap = !Swap;
	}



	//effects the terrain with noise when needed
	if (NoiseOn)
	{
		for (size_t i = 0; i < NumVertZ; i++)
		{
			for (size_t j = 0; j < NumVertX; j++)
			{
				NoiseValue = Noise(i, j);
				NoiseValue = NoiseValue + 1.25 / 2;
				glm::vec3 NoiseVec = TerrainVerts[Index];

				if (ExtraNoise)
				{
					NoiseValue = NoiseValue * 2;
				}

				NoiseVec.y = NoiseVec.y + NoiseValue;

				TerrainVerts[Index] = NoiseVec;
				Index++;

			}
		}
	}
	//terrain normals
	for (glm::vec3& n : Normals)
		n = glm::vec3(0, 0, 0);

	for (size_t Index = 0; Index < TerrainElements.size(); Index += 3)
	{
		glm::vec3 V0{ TerrainVerts[TerrainElements[Index]] };
		glm::vec3 V1{ TerrainVerts[TerrainElements[Index + 1]] };
		glm::vec3 V2{ TerrainVerts[TerrainElements[Index + 2]] };

		glm::vec3 Side1 = V1 - V0;
		glm::vec3 Side2 = V2 - V0;

		glm::vec3 TriNormal = glm::cross(Side1, Side2);
		glm::normalize(TriNormal);

		Normals[TerrainElements[Index]] += TriNormal;
		Normals[TerrainElements[Index + 1]] += TriNormal;
		Normals[TerrainElements[Index + 2]] += TriNormal;
	}

	for (glm::vec3& n : Normals)
		glm::normalize(n);
	//Terrain texture loader
	Helpers::ImageLoader TexLoader;
	if (TexLoader.Load("Data\\Textures\\dirt_earth-n-moss_df_.dds"))
	{
		glGenTextures(1, &TerrainTex);
		glBindTexture(GL_TEXTURE_2D, TerrainTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TexLoader.Width(), TexLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, TexLoader.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		MessageBox(NULL, L"Texture not loading", L"Error",
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}


	//loads the Positions to be warpped
	GLuint TerrainPositionsVBO;
	glGenBuffers(1, &TerrainPositionsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, TerrainPositionsVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * TerrainVerts.size(), TerrainVerts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//loads the UVcoords to be warpped
	GLuint TerrainCoordsVBO;
	glGenBuffers(1, &TerrainCoordsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, TerrainCoordsVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * UVCoords.size(), UVCoords.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//loads the normals to be warpped
	GLuint NormalsVBO;
	glGenBuffers(1, &NormalsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Normals.size(), Normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//loads the elements to be warpped
	GLuint TerrainElementEBO;
	glGenBuffers(1, &TerrainElementEBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TerrainElementEBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * TerrainElements.size(), TerrainElements.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_numTerrainElements = TerrainElements.size();



	glGenVertexArrays(1, &m_TerrainVAO);

	//warps positions
	glBindVertexArray(m_TerrainVAO);

	glBindBuffer(GL_ARRAY_BUFFER, TerrainPositionsVBO);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	//warps elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TerrainElementEBO);

	glBindVertexArray(0);

	//warps UVcoords
	glBindVertexArray(m_TerrainVAO);

	glBindBuffer(GL_ARRAY_BUFFER, TerrainCoordsVBO);

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindVertexArray(0);
	//warps normals
	glBindVertexArray(m_TerrainVAO);

	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);

	glEnableVertexAttribArray(2);

	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindVertexArray(0);

	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{			
	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 4500.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;
	
	//skybox
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(SkyBox_program);

	glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));
	glm::mat4 combined_xform2 = projection_xform * view_xform2;
	GLuint Skycombined_xform_id = glGetUniformLocation(SkyBox_program, "combined_xform2");
	glUniformMatrix4fv(Skycombined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform2));

	glm::mat4 model_xform = glm::mat4(1);
	GLuint Skymodel_xform_id = glGetUniformLocation(SkyBox_program, "model_xform");
	glUniformMatrix4fv(Skymodel_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	GLuint SkyboxUniformID = glGetUniformLocation(SkyBox_program, "skybox");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);

	glBindVertexArray(m_SkyBoxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);

	//Jeep program
	glUseProgram(m_program);

	GLuint Jeepcombined_xform_id = glGetUniformLocation(m_program, "Jeepcombined_xform");
	glUniformMatrix4fv(Jeepcombined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));


	GLuint Jeepmodel_xform_id = glGetUniformLocation(m_program, "Jeepmodel_xform");
	glUniformMatrix4fv(Jeepmodel_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, JeepTex);
	glUniform1i(glGetUniformLocation(m_program, "JeepSamplerTex"), 0);
	glBindVertexArray(m_JeepVAO);
	glDrawElements(GL_TRIANGLES, m_NumJeepElements, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);

	//Terrain program
	glUseProgram(TerrainProgram);

	GLuint Terraincombined_xform_id = glGetUniformLocation(TerrainProgram, "Terraincombined_xform");
	glUniformMatrix4fv(Terraincombined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	GLuint Terrainmodel_xform_id = glGetUniformLocation(TerrainProgram, "Terrainmodel_xform");
	glUniformMatrix4fv(Terrainmodel_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TerrainTex);
	glUniform1i(glGetUniformLocation(TerrainProgram, "TerrainSamplerTex"), 0);
	glBindVertexArray(m_TerrainVAO);
	glDrawElements(GL_TRIANGLES, m_numTerrainElements, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
	
	//Cube program
	glUseProgram(CubeProgram);

	model_xform = glm::translate(model_xform, glm::vec3{ 1000.0f, 500.0f, 500.0f });
	// Uncomment all the lines below to rotate cube first round y then round x
	static float angle = 0;
	static bool rotateY = true;

	if (rotateY) // Rotate around y axis		
		model_xform = glm::rotate(model_xform, angle, glm::vec3{ 0 ,1,0 });
	else // Rotate around x axis		
		model_xform = glm::rotate(model_xform, angle, glm::vec3{ 1 ,0,0 });

	angle += 0.0025f;
	if (angle > glm::two_pi<float>())
	{
		angle = 0;
		rotateY = !rotateY;
	}

	GLuint Cubecombined_xform_id = glGetUniformLocation(CubeProgram, "Cubecombined_xform");
	glUniformMatrix4fv(Cubecombined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	GLuint Cubemodel_xform_id = glGetUniformLocation(CubeProgram, "Cubemodel_xform");
	glUniformMatrix4fv(Cubemodel_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
	
	glBindVertexArray(m_CubeVAO);
	glDrawElements(GL_TRIANGLES, m_numCubeElements, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);

	

}

