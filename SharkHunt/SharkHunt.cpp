// Fall 2019

#include "SharkHunt.hpp"
#include "scene_lua.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>
#include <lodepng/lodepng.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
using namespace glm;

static bool show_gui = true;
const size_t CIRCLE_PTS = 48;
const float SKYBOX_SIZE = 30.0f;

//----------------------------------------------------------------------------------------
// Constructor
SharkHunt::SharkHunt(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
SharkHunt::~SharkHunt()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void SharkHunt::init()
{
	// Set the background colour.
	glClearColor(0.85, 0.85, 0.85, 1.0);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	createShaderProgram();

	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj"),
			getAssetFilePath("Models/Bomb.obj"),
			getAssetFilePath("Models/Weapon_PortableCannon.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	initCamera();
	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	zbuffer = true;
	backface_culling = false;
	frontface_culling = false;

	player = new Player(m_rootNode);
	initCannonball();

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
	loadTexture();
	loadSkybox();
}

void SharkHunt::initCamera() {
	pitch = 0.0f;
	yaw = -90.0f;
	sensitivity = 0.1f;

	cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	// Define speed of camera
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;  

	cameraSpeed = 0.25f * deltaTime;

	// TODO: Set to the center of the screen in more sophisticated way
	old_x_pos = m_windowWidth / 2; 
	old_y_pos = m_windowHeight / 2;
}

void SharkHunt::loadSkybox() {
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	vector<string> faces {
		getAssetFilePath("skybox/sor_lake1/lake1_rt.png"),
		getAssetFilePath("skybox/sor_lake1/lake1_lf.png"),
		getAssetFilePath("skybox/sor_lake1/lake1_up.png"),
		getAssetFilePath("skybox/sor_lake1/lake1_dn.png"),
		getAssetFilePath("skybox/sor_lake1/lake1_bk.png"),
		getAssetFilePath("skybox/sor_lake1/lake1_ft.png"),
	};

	for (GLuint i = 0; i < faces.size(); i++) {
		vector<unsigned char> cubemap;  
		unsigned width, height;
		
		//decode
		unsigned error = lodepng::decode(cubemap, width, height, faces[i].c_str());

		//if there's an error, display it
		if(error) cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
			0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubemap[0]
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Bind things
	float verts[] = {
      -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
      -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
      -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,

      -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
      -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
      -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
      -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
      -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
      -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,

       SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,

      -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
      -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
      -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,

      -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
      -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
      -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,

      -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
      -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
      -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
       SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE
    };

	glGenVertexArrays(1, &m_vao_skybox);
	glBindVertexArray(m_vao_skybox);

	glGenBuffers(1, &m_vbo_skybox);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);
	CHECK_GL_ERRORS;
}

void SharkHunt::loadTexture() {
	// GLuint texture;
	glGenTextures(1, &tntTexture);
	glBindTexture(GL_TEXTURE_2D, tntTexture);

	// Example code from lodepng for decoding
	vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, getAssetFilePath("Models/red.png"));

	//if there's an error, display it
	if(error) cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// glActiveTexture(0);
	// glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void SharkHunt::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = import_lua(filename);
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void SharkHunt::createShaderProgram() {
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_skybox_shader.generateProgramObject();
	m_skybox_shader.attachVertexShader( getAssetFilePath("SkyboxShader.vs").c_str() );
	m_skybox_shader.attachFragmentShader( getAssetFilePath("SkyboxShader.fs").c_str() );
	m_skybox_shader.link();
}

//----------------------------------------------------------------------------------------
void SharkHunt::enableVertexShaderInputSlots() {
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		// Enable the vertex shader attribute location for "uv" when rendering.
		m_uvAttribLocation = m_shader.getAttribLocation("uv");
		glEnableVertexAttribArray(m_uvAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void SharkHunt::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex UV data
	{
		glGenBuffers(1, &m_vbo_uvCoords);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_uvCoords);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUVBytes(),
				meshConsolidator.getVertexUVDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void SharkHunt::mapVboDataToVertexShaderInputLocations() {
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Tell GL how to map data from the vertex buffer "m_vbo_uvCoords" into the
	// "uv" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_uvCoords);
	glVertexAttribPointer(m_uvAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//-- Unbind target, and restore default values:
	// glDisableVertexAttribArray(m_positionAttribLocation);
	// glDisableVertexAttribArray(m_normalAttribLocation);
	// glDisableVertexAttribArray(m_vbo_uvCoords);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void SharkHunt::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void SharkHunt::initViewMatrix() {
	m_view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

//----------------------------------------------------------------------------------------
void SharkHunt::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void SharkHunt::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.25f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set texture
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tntTexture);

			glUniform1i(m_shader.getUniformLocation("sampler"), 0);
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void SharkHunt::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();

	// WASD Player Movements in First-Person
	// Constantly poll to see if the key is being held down or not
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * vec3(cameraFront.x, 0, cameraFront.z);
		// cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	} 
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * vec3(cameraFront.x, 0, cameraFront.z);
		// cameraPos -= cameraSpeed * cameraFront;
	} 
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void SharkHunt::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		ImGui::Text( "Camera: %.1f Pitch %.1f Yaw", pitch, yaw );
		ImGui::Text( "Camera Pos: (%.1f, %.1f, %.1f) ", cameraPos.x, cameraPos.y, cameraPos.z );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void SharkHunt::draw() {

	// Update camera
	m_view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	if (zbuffer) { glEnable( GL_DEPTH_TEST ); }
	enableCulling();
	renderCannonball();
	renderSceneGraph();
	renderSkybox();
	disableCulling(); 
	if (zbuffer) { glDisable( GL_DEPTH_TEST ); }
}

// TODO: Clean up; I think this pattern has memory leaks: shared_ptrs?
void SharkHunt::initCannonball() {
	cannonball = new Weapon(import_lua(getAssetFilePath("cannonball.lua")));
}

void SharkHunt::renderCannonball() {
	glBindVertexArray(m_vao_meshData);

	deque<glm::mat4> stack;

	if (player->isShooting) {
		cannonball->animate();
		cannonball->model->render(m_shader, m_view, m_batchInfoMap, stack);
		// If Cannonball has reached the end of its lifetime, generate a new one 
		// with a fresh model 
		// TODO: This is mostly a hack, clean it up at the end
		if (cannonball->isExpired()) {
			delete cannonball;
			initCannonball();
			player->setAmmo(1);
			player->isShooting = false;
		}
	}

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void SharkHunt::renderSkybox() {
	glDepthMask(GL_FALSE);

	mat4 view = mat4(mat3(m_view));
	m_skybox_shader.enable();
		glUniformMatrix4fv(m_skybox_shader.getUniformLocation("V"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(m_skybox_shader.getUniformLocation("P"), 1, GL_FALSE, value_ptr(m_perpsective));
		glUniform1i(m_skybox_shader.getUniformLocation("skybox"), 0);
	m_skybox_shader.disable();

	GLuint positionAttrib = m_skybox_shader.getAttribLocation("aPos");

	glBindVertexArray(m_vao_skybox);
	glEnableVertexAttribArray(positionAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	m_skybox_shader.enable();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	m_skybox_shader.disable();

	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
}

void SharkHunt::enableCulling()
{
	if (backface_culling && !frontface_culling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	} else if (frontface_culling && !backface_culling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	} else if (frontface_culling && backface_culling){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

void SharkHunt::disableCulling()
{
	glDisable(GL_CULL_FACE);
}

//----------------------------------------------------------------------------------------
void SharkHunt::renderSceneGraph() {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	deque<glm::mat4> stack;
	
	m_rootNode->render(m_shader, m_view, m_batchInfoMap, stack);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void SharkHunt::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool SharkHunt::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool SharkHunt::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

    if (firstMouseMove) {
        old_x_pos = xPos;
        old_y_pos = yPos;
        firstMouseMove = false;
    }
  
    float xoffset = xPos - old_x_pos;
    float yoffset = old_y_pos - yPos; 
    old_x_pos = xPos;
    old_y_pos = yPos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

	pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool SharkHunt::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
		player->shoot();
		cannonball->setTrajectory(cameraPos, cameraFront);
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool SharkHunt::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool SharkHunt::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool SharkHunt::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		switch (key) {
			// Quit
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(m_window, GL_TRUE);
				break;
		}
	}

	return eventHandled;
}
