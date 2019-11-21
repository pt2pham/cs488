// Fall 2019

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.85, 0.85, 0.85, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
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
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	transformation_mode = 1; // Start in Position/Orientation (P) mode
	circle = false;
	zbuffer = true;
	backface_culling = false;
	frontface_culling = false;
	init_root_trans = m_rootNode->get_transform();

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

// TODO: Isolate translation and rotation transformation matrices 
void A3::resetPosition() {
	m_rootNode->set_transform(init_root_trans);
}

void A3::resetOrientation() {
	m_rootNode->set_transform(init_root_trans);
}

// TODO: Did not implement
void A3::resetJoints() {

}

// TODO: Did not implement
void A3::undo() {

}

// TODO: Did not implement
void A3::redo() {

}


//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
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

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	// m_shader.enable();
	// {
	// 	//-- Set Perpsective matrix uniform for the scene:
	// 	GLint location = m_shader.getUniformLocation("Perspective");
	// 	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
	// 	CHECK_GL_ERRORS;


	// 	//-- Set LightSource uniform for the scene:
	// 	{
	// 		location = m_shader.getUniformLocation("light.position");
	// 		glUniform3fv(location, 1, value_ptr(m_light.position));
	// 		location = m_shader.getUniformLocation("light.rgbIntensity");
	// 		glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
	// 		CHECK_GL_ERRORS;
	// 	}

	// 	//-- Set background light ambient intensity
	// 	{
	// 		location = m_shader.getUniformLocation("ambientIntensity");
	// 		vec3 ambientIntensity(0.25f);
	// 		glUniform3fv(location, 1, value_ptr(ambientIntensity));
	// 		CHECK_GL_ERRORS;
	// 	}
	// }
	// m_shader.disable();

	// From PickingExample
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, do_picking ? 1 : 0 );

		//-- Set LightSource uniform for the scene:
		if( !do_picking ) {
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;

			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
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
		if( ImGui::BeginMainMenuBar() ) {
			if( ImGui::BeginMenu("Application")) {
				if( ImGui::MenuItem( "Reset Position (I)" ) ) {
					resetPosition();
				}
				if( ImGui::MenuItem( "Reset Orientation (O)" ) ) {
					resetOrientation();
				}
				if( ImGui::MenuItem( "Reset Joints (J)" ) ) {
					resetJoints();
				}
				if( ImGui::MenuItem( "Reset All (A)" ) ) {
					resetJoints();
					resetOrientation();
					resetPosition();
				}
				ImGui::EndMenu();
			}
			if( ImGui::BeginMenu("Edit")) {
				if( ImGui::MenuItem( "Undo (U)" ) ) {
					undo();
				}
				if( ImGui::MenuItem( "Redo (R)" ) ) {
					redo();
				}
				ImGui::EndMenu();
			}
			if( ImGui::BeginMenu("Options")) {
				if( ImGui::MenuItem( "Circle (C)" ) ) {
					circle = !circle;
				}
				if( ImGui::MenuItem( "Z-Buffer (Z)" ) ) {
					zbuffer = !zbuffer;
				}
				if( ImGui::MenuItem( "Backface Culling (B))" ) ) {
					backface_culling = !backface_culling;
				}
				if( ImGui::MenuItem( "Frontface Culling (F)" ) ) {
					frontface_culling = !frontface_culling;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::RadioButton( "Position/Orientation (P)", &transformation_mode, 1 );
		ImGui::RadioButton( "Joints (J)", &transformation_mode, 2 );


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (zbuffer) { glEnable( GL_DEPTH_TEST ); }
	enableCulling();
	renderSceneGraph();
	disableCulling(); // TODO: Does this sandwich need to exist
	if (zbuffer) { glDisable( GL_DEPTH_TEST ); }

	if (circle) { renderArcCircle(); }
}

void A3::enableCulling()
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

void A3::disableCulling()
{
	glDisable(GL_CULL_FACE);
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph() {

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
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	float diffX = xPos - old_x_pos;
	float diffY = yPos - old_y_pos;
	switch (transformation_mode) { // Choose which thing to use based on mode
		case 1: // Update Position/Orientation
			updatePosition(diffX);
			eventHandled = true;
			break;
		case 2: // Update Joints
			updateJoints(diffY);
			eventHandled = true;
			break;
	}
	old_x_pos = xPos;
	old_y_pos = yPos;

	return eventHandled;
}

void A3::updatePosition(double diff) {
	// if (ImGui::IsMouseDown(0)) {
	// 	const mat4 translation_X(
	// 		1, 0, 0, 0,
	// 		0, 1, 0, 0,
	// 		0, 0, 1, 0,
	// 		diff*0.1, 0, 0, 1
	// 	);
	// 	V = inverse(translation_X) * V;
	// }
	// if (ImGui::IsMouseDown(2)) {
	// 	const mat4 translation_Z(
	// 		1, 0, 0, 0,
	// 		0, 1, 0, 0,
	// 		0, 0, 1, 0,
	// 		0, 0, diff*0.1, 1
	// 	);
	// 	V = inverse(translation_Z) * V;
	// }
	// double theta = degreesToRadians(diff);
	// if (ImGui::IsMouseDown(1)) {
	// 	const mat4 rotation_Y(
	// 		std::cos(theta), 0, -std::sin(theta), 0,
	// 		0, 1, 0, 0,
	// 		std::sin(theta), 0, std::cos(theta), 0,
	// 		0, 0, 0, 1
	// 	);
	// 	M = M * rotation_Y;
	// }

	if (ImGui::IsMouseDown(0)) { // Left Click
		m_rootNode->translate(vec3(diff*0.1, 0, 0));
	}
	if (ImGui::IsMouseDown(2)) { // Middle Click
		m_rootNode->translate(vec3(0, 0, diff*0.1));
	}
	if (ImGui::IsMouseDown(1)) { // Right Click
		SceneNode * torsoNode = m_rootNode->find_node("torso");
		torsoNode->rotate('y', diff);
	}
}

void A3::updateJoints(double diff) {
	if (ImGui::IsMouseDown(2)) { // Left Click
		// Rotate all selected joints
		for (auto jointNode : selectedJointNodes)
			jointNode->rotate('x', diff);
	}
	if (ImGui::IsMouseDown(1)) { // Right Click
		// Rotate head left to right
		SceneNode * headNode = m_rootNode->find_node("head");
		SceneNode * headJoint = headNode->parent;
		for (auto jointNode : selectedJointNodes) {
			if (jointNode->m_nodeId == headJoint->m_nodeId)
				headJoint->rotate('y', diff );
		}
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS && transformation_mode == 2) {
		double xpos, ypos;
		glfwGetCursorPos( m_window, &xpos, &ypos );

		do_picking = true;
		m_rootNode->picking_on(); // Tell hierarchy tree that we're picking

		uploadCommonSceneUniforms();
		glClearColor(1.0, 1.0, 1.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClearColor(0.35, 0.35, 0.35, 1.0);

		draw();

		CHECK_GL_ERRORS;

		// Ugly -- FB coordinates might be different than Window coordinates
		// (e.g., on a retina display).  Must compensate.
		xpos *= double(m_framebufferWidth) / double(m_windowWidth);
		// WTF, don't know why I have to measure y relative to the bottom of
		// the window in this case.
		ypos = m_windowHeight - ypos;
		ypos *= double(m_framebufferHeight) / double(m_windowHeight);

		GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
		// A bit ugly -- don't want to swap the just-drawn false colours
		// to the screen, so read from the back buffer.
		glReadBuffer( GL_BACK );
		// Actually read the pixel at the mouse location.
		glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
		CHECK_GL_ERRORS;

		// Reassemble the object ID.
		unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

		if( what < m_rootNode->totalSceneNodes() ) {
			// Select the node with that ID
			auto node = m_rootNode->find_node(what); // Should be a GeometryNode
			node->isSelected = !node->isSelected;

			SceneNode * selected_node = node;
			while (selected_node->parent != nullptr)
			{
				// If the parent of the picked node is not a JointNode, we assume that they're
				// part of the same hierarchy/should be picked together. I.e they're not meant to be 
				// manipulated separately by the joint
				SceneNode * parent = selected_node->parent;
				if (parent->m_nodeType != NodeType::JointNode) { 
					parent->isSelected = !parent->isSelected;
					// Keep looping up the tree via parent until we hit a JointNode
				} else if (parent->m_nodeType == NodeType::JointNode ) {
					// Push this node into our vector of selectedJoints that will 
					// be transformed
					if (parent->m_name != "head") { // Don't store head
						if (selected_node->isSelected) {
							selectedJointNodes.insert(parent);
						} else {
							selectedJointNodes.erase(parent);
						}
					}
					break;
				}
				selected_node = parent;
			}
		}

		do_picking = false;
		m_rootNode->picking_off();

		CHECK_GL_ERRORS;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
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
bool A3::keyInputEvent (
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
			// Resets
			case GLFW_KEY_A: // Reset All
				transformation_mode = 1; // Start in Position/Orientation (P) mode
				resetPosition();
				resetOrientation();
				resetJoints();
				eventHandled = true;
				break;
			case GLFW_KEY_I:
				resetPosition();
				eventHandled = true;
				break;
			case GLFW_KEY_O:
				resetOrientation();
				eventHandled = true;
				break;
			case GLFW_KEY_U: // Undo
				undo();
				eventHandled = true;
				break;
			case GLFW_KEY_R: // Redo
				redo();
				eventHandled = true;
				break;
			// Transformation Modes
			case GLFW_KEY_P: // Change Position/Orientation
				transformation_mode = 1;
				eventHandled = true;
				break;
			case GLFW_KEY_J: // Change Joints
				transformation_mode = 2;
				eventHandled = true;
				break;
			// Options Menu Commands
			case GLFW_KEY_C: // Toggle Circle
				circle = !circle;
				eventHandled = true;
				break;
			case GLFW_KEY_Z: // Toggle Z-Buffer
				zbuffer = !zbuffer;
				eventHandled = true;
				break;
			case GLFW_KEY_B: // Toggle Backface Culling
				backface_culling = !backface_culling;
				eventHandled = true;
				break;
			case GLFW_KEY_F: // Toggle Frontface Culling
				frontface_culling = !frontface_culling;
				eventHandled = true;
				break;
			case GLFW_KEY_M: // Toggle GUI
				show_gui = !show_gui;
				eventHandled = true;
				break;
			default:
				break;
		}
	}

	return eventHandled;
}
