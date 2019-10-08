// Fall 2019

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{
	cube = {
		{glm::vec4(-1.0f, -1.0f, -1.0f, 1), glm::vec4( 1.0f, -1.0f, -1.0f, 1)},
		{glm::vec4(-1.0f, -1.0f, -1.0f, 1), glm::vec4(-1.0f, -1.0f,  1.0f, 1)},
		{glm::vec4( 1.0f, -1.0f,  1.0f, 1), glm::vec4( 1.0f, -1.0f, -1.0f, 1)},
		{glm::vec4( 1.0f, -1.0f,  1.0f, 1), glm::vec4(-1.0f, -1.0f,  1.0f, 1)},

		{glm::vec4(-1.0f,  1.0f, -1.0f, 1), glm::vec4( 1.0f,  1.0f, -1.0f, 1)},
		{glm::vec4(-1.0f,  1.0f, -1.0f, 1), glm::vec4(-1.0f,  1.0f,  1.0f, 1)},
		{glm::vec4( 1.0f,  1.0f,  1.0f, 1), glm::vec4( 1.0f,  1.0f, -1.0f, 1)},
		{glm::vec4( 1.0f,  1.0f,  1.0f, 1), glm::vec4(-1.0f,  1.0f,  1.0f, 1)},
			
		{glm::vec4(-1.0f, -1.0f, -1.0f, 1), glm::vec4(-1.0f,  1.0f, -1.0f, 1)},
		{glm::vec4(-1.0f, -1.0f,  1.0f, 1), glm::vec4(-1.0f,  1.0f,  1.0f, 1)},
		{glm::vec4( 1.0f, -1.0f, -1.0f, 1), glm::vec4( 1.0f,  1.0f, -1.0f, 1)},
		{glm::vec4( 1.0f, -1.0f,  1.0f, 1), glm::vec4( 1.0f,  1.0f,  1.0f, 1)}
	};

	world_gnomon = {
		{vec4(0, 0, 0, 1), vec4(0.5, 0, 0, 1)},
		{vec4(0, 0, 0, 1), vec4(0, 0.5, 0, 1)},
		{vec4(0, 0, 0, 1), vec4(0, 0, 0.5, 1)},
	};

	model_gnomon = {
		{vec4(0, 0, 0, 1), vec4(0.5, 0, 0, 1)},
		{vec4(0, 0, 0, 1), vec4(0, 0.5, 0, 1)},
		{vec4(0, 0, 0, 1), vec4(0, 0, 0.5, 1)},
	};
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	transformation_mode = 4 ; // Default is 'Rotate Model'

	M = make_mat4(initM);
	M_gnomon = make_mat4(initM);
	initV();
	initP();

	viewportWidth = 0.9 * m_framebufferWidth;
	viewportHeight = 0.9 * m_framebufferHeight;
	viewport_x_vl = -0.9f;
	viewport_y_vb = -0.9f;

	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
}

void A2::initV() {
	V = mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 8, 1
	);
}

void A2::initP() {
	float aspect = float( m_framebufferWidth ) / float( m_framebufferHeight ); // From A1
	float theta = glm::radians(fieldOfView);
	float cot = cos(theta/2) / sin(theta/2);
	
	P = mat4(
		cot / aspect, 0, 0, 0,
		0, cot, 0, 0,
		0, 0, (far + near)/(far - near), 1,
		0, 0, -2 * far * near / (far - near), 0
	);

	// P = mat4(
	// 	1,0,0,0,
	// 	0,1,0,0,
	// 	0,0,1,1,
	// 	0,0,1,0
	// );
}

void A2::reset() {
	transformation_mode = 4; // Default is Rotate Model mode

	viewportWidth = 0.9 * m_framebufferWidth;
	viewportHeight = 0.9 * m_framebufferHeight;
	viewport_x_vl = -0.9f;
	viewport_y_vb = -0.9f;

	near = 1.0f;
	far = 1000.0f;
	fieldOfView = 30.0f;

	M = make_mat4(initM);
	M_gnomon = make_mat4(initM);
	initV();
	initP();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2019
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

glm::vec2 A2::scaleToViewport(const glm::vec2 & v) {
	float x_wl = -1;
	float y_wb = -1;

	return vec2(
		(viewportWidth / m_framebufferWidth) * (v.x - x_wl) + viewport_x_vl, 
		(viewportHeight / m_framebufferHeight) * (v.y - y_wb) + viewport_y_vb
	);
}

void A2::drawViewport() {
	// Draw viewport
	setLineColour(vec3(1.0f, 1.0f, 1.0f));

	// Bottom left to bottom right
	vec2 bot_left = vec2(viewport_x_vl, viewport_y_vb);
	vec2 bot_right = vec2(
		viewport_x_vl + 2 * float(viewportWidth / m_framebufferWidth), 
		viewport_y_vb)
	;
	vec2 top_left = vec2(
		viewport_x_vl, 
		viewport_y_vb + 2 * float(viewportHeight / m_framebufferHeight)
	);
	vec2 top_right = vec2(
		viewport_x_vl + 2 * float(viewportWidth / m_framebufferWidth), 
		viewport_y_vb + 2 * float(viewportHeight / m_framebufferHeight)
	);

	drawLine(bot_left, bot_right);
	drawLine(bot_right, top_right);
	drawLine(top_right, top_left);
	drawLine(top_left, bot_left);
}

void A2::drawCube() {
	for (auto vertex_pair : cube) { // Draw Cube after going through transformation pipeline
		setLineColour(vec3(1.0f, 0.5f, 0.25f));
		vec4 start = P * V * M * vertex_pair.first;
		vec4 end = P * V * M * vertex_pair.second;
		if (clip(start, end)) {
			drawLine(
				scaleToViewport(vec2(start.x / start.w, start.y / start.w)), 
				scaleToViewport(vec2(end.x / end.w, end.y / end.w))
			);
		}
	}
}

void A2::drawGnomons() {
	// Draw Model Gnomon
	for (int i = 0; i < 3; ++i) {
		switch (i) {
			case 0:
				setLineColour(vec3(1.0f, 0.0f, 0.0f));
				break;
			case 1:
				setLineColour(vec3(0.0f, 1.0f, 0.0f));
				break;
			case 2:
				setLineColour(vec3(0.0f, 0.0f, 1.0f));
				break;
		}

		vec4 start = P * V * M_gnomon * model_gnomon[i].first;
		vec4 end = P * V * M_gnomon * model_gnomon[i].second;
		if (clip(start, end)) {
			drawLine(
				scaleToViewport(vec2(start.x / start.w, start.y / start.w)), 
				scaleToViewport(vec2(end.x / end.w, end.y / end.w))
			);
		}
	}

	// Draw World Gnomon
	for (int i = 0; i < 3; ++i) {
		switch (i) {
			case 0:
				setLineColour(vec3(1.0f, 1.0f, 0.0f));
				break;
			case 1:
				setLineColour(vec3(0.0f, 1.0f, 1.0f));
				break;
			case 2:
				setLineColour(vec3(1.0f, 0.0f, 1.0f));
				break;
		}

		vec4 start = P * V * world_gnomon[i].first;
		vec4 end = P * V * world_gnomon[i].second;
		if (clip(start, end)) {
			drawLine(
				scaleToViewport(vec2(start.x / start.w, start.y / start.w)), 
				scaleToViewport(vec2(end.x / end.w, end.y / end.w))
			);
		}
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();
	drawViewport();
	drawCube();
	drawGnomons();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
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
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset" ) ) {
			reset();
		}

		// Add more gui elements here here ...
		ImGui::RadioButton( "Rotate View", &transformation_mode, 1 );
		ImGui::RadioButton( "Translate View", &transformation_mode, 2 );
		ImGui::RadioButton( "Perspective", &transformation_mode, 3 );
		ImGui::RadioButton( "Rotate Model", &transformation_mode, 4 );
		ImGui::RadioButton( "Translate Model", &transformation_mode, 5 );
		ImGui::RadioButton( "Scale Model", &transformation_mode, 6 );
		ImGui::RadioButton( "Viewport", &transformation_mode, 7 );

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		ImGui::Text( "FoV: %.1f", fieldOfView );
		ImGui::Text( "Near plane: %.1f", near );
		ImGui::Text( "Far plane: %.1f", far );
		ImGui::Text( "Window Height: %d", m_framebufferHeight );
		ImGui::Text( "Window Width: %d", m_framebufferWidth );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	float diff = xPos - old_x_pos;
	switch (transformation_mode) { // Choose which thing to use based on mode
		case 1:
			rotateView(diff);
			eventHandled = true;
			break;
		case 2:
			translateView(diff);
			eventHandled = true;
			break;
		case 3:
			changePerspective(diff);
			eventHandled = true;
			break;
		case 4:
			rotateModel(diff);
			eventHandled = true;
			break;
		case 5:
			translateModel(diff);
			eventHandled = true;
			break;
		case 6:
			scaleModel(diff);
			eventHandled = true;
			break;
		case 7:
			resizeViewport(xPos, yPos);
			eventHandled = true;
			break;
	}
	old_x_pos = xPos;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (transformation_mode == 7) {  // Viewport
			storeViewportCorner = true;
		}
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
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
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		switch (key) {
			case GLFW_KEY_Q: // Quit
				glfwSetWindowShouldClose(m_window, GL_TRUE);
				break;
			case GLFW_KEY_A: // Reset
				reset();
				eventHandled = true;
				break;
			case GLFW_KEY_O: // Rotate View
				transformation_mode = 1;
				eventHandled = true;
				break;
			case GLFW_KEY_E: // Translate View
				transformation_mode = 2;
				eventHandled = true;
				break;
			case GLFW_KEY_P: // Change Perspective
				transformation_mode = 3;
				eventHandled = true;
				break;
			case GLFW_KEY_R: // Rotate Model
				transformation_mode = 4;
				eventHandled = true;
				break;
			case GLFW_KEY_T: // Translate Model
				transformation_mode = 5;
				eventHandled = true;
				break;
			case GLFW_KEY_S: // Scale Model
				transformation_mode = 6;
				eventHandled = true;
				break;
			case GLFW_KEY_V: // Resize Viewport
				transformation_mode = 7;
				eventHandled = true;
				break;
			default:
				break;
		}
	}
	return eventHandled;
}

void A2::rotateModel(double xDifference) {
	double theta = glm::radians(xDifference);

	if (ImGui::IsMouseDown(0)) {
		const mat4 rotation_X(
			1, 0, 0, 0,
			0, std::cos(theta), std::sin(theta), 0,
			0, -std::sin(theta), std::cos(theta), 0,
			0, 0, 0, 1
		);
		M = M * rotation_X;
		M_gnomon = M_gnomon * rotation_X;
	}

	if (ImGui::IsMouseDown(2)) {
		const mat4 rotation_Y(
			std::cos(theta), 0, -std::sin(theta), 0,
			0, 1, 0, 0,
			std::sin(theta), 0, std::cos(theta), 0,
			0, 0, 0, 1
		);
		M = M * rotation_Y;
		M_gnomon = M_gnomon * rotation_Y;
	}

	if (ImGui::IsMouseDown(1)) {
		const mat4 rotation_Z(
			std::cos(theta), std::sin(theta), 0, 0,
			-std::sin(theta), std::cos(theta), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
		M = M * rotation_Z;
		M_gnomon = M_gnomon * rotation_Z;
	}
}

void A2::translateModel(double xDifference) {
	float diff = xDifference * 0.01f;

	if (ImGui::IsMouseDown(0)) {
		const mat4 translation_X(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			diff, 0, 0, 1
		);
		M = M * translation_X;
		M_gnomon = M_gnomon * translation_X;
	}

	if (ImGui::IsMouseDown(2)) {
		const mat4 translation_Y(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, diff, 0, 1
		);
		M = M * translation_Y;
		M_gnomon = M_gnomon * translation_Y;
	}

	if (ImGui::IsMouseDown(1)) {
		const mat4 translation_Z(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, diff, 1
		);
		M = M * translation_Z;
		M_gnomon = M_gnomon * translation_Z;
	}
}

/*
 * Don't affect the model gnomon!
 */
void A2::scaleModel(double xDifference) {
	float scale = xDifference > 0 ? 1.01f : 0.99f;

	if (ImGui::IsMouseDown(0)) {
		const mat4 scale_X(
			scale, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
		M = M * scale_X;
	}

	if (ImGui::IsMouseDown(2)) {
		const mat4 scale_Y(
			1, 0, 0, 0,
			0, scale, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
		M = M * scale_Y;
	}

	if (ImGui::IsMouseDown(1)) {
		const mat4 scale_Z(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, scale, 0,
			0, 0, 0, 1
		);
		M = M * scale_Z;
	}
}

void A2::rotateView(double xDifference) {
	// rotateModel but inverse matrices
	double theta = glm::radians(xDifference);
	// Strangely, these 2 transformations are affecting the wrong axis.
	if (ImGui::IsMouseDown(2)) { 
		const mat4 rotation_X(
			1, 0, 0, 0,
			0, std::cos(theta), std::sin(theta), 0,
			0, -std::sin(theta), std::cos(theta), 0,
			0, 0, 0, 1
		);
		V = inverse(rotation_X) * V;
	}
	// Strangely, these 2 transformations are affecting the wrong axis.
	if (ImGui::IsMouseDown(0)) { 
		const mat4 rotation_Y(
			std::cos(theta), 0, -std::sin(theta), 0,
			0, 1, 0, 0,
			std::sin(theta), 0, std::cos(theta), 0,
			0, 0, 0, 1
		);
		V = inverse(rotation_Y) * V;
	}

	if (ImGui::IsMouseDown(1)) {
		const mat4 rotation_Z(
			std::cos(theta), std::sin(theta), 0, 0,
			-std::sin(theta), std::cos(theta), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
		V = inverse(rotation_Z) * V;
	}
}

void A2::translateView(double xDifference) {
	// translateModel but inverse matrices
	float diff = xDifference * 0.01f;

	if (ImGui::IsMouseDown(0)) {
		const mat4 translation_X(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			diff, 0, 0, 1
		);
		V = inverse(translation_X) * V;
	}

	if (ImGui::IsMouseDown(2)) {
		const mat4 translation_Y(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, diff, 0, 1
		);
		V = inverse(translation_Y) * V;
	}

	if (ImGui::IsMouseDown(1)) {
		const mat4 translation_Z(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, diff, 1
		);
		V = inverse(translation_Z) * V;
	}
}

void A2::changePerspective(double xDifference) {
	if (ImGui::IsMouseDown(0)) { // FoV change
		fieldOfView += xDifference * 0.05f;
		fieldOfView = clamp(fieldOfView, 5.0f, 160.0f);
	}

	if (ImGui::IsMouseDown(2)) { // Translate Near plane
		near += xDifference * 0.05f;
	}

	if (ImGui::IsMouseDown(1)) { // Translate Far plane
		far += xDifference * 0.05f;
	}
	initP();
}

/*
 *
 * This function is
 * INCOMPLETE!
 * 
 * I think I see the flaw in this because I've made the viewport rely on 
 * knowing the bottom left corner, but getting the bottom left corner from
 * this is pretty difficult/cumbersome.
 * 
 * Maybe an alternative is to just calculate the length and width and build the
 * viewport based on whatever coordinate was selected here and whatever it ended on?
 * 
 * So in the initialization case, we choose 2 vertices diagonal from each other to build
 * 
 * Then, it generalizes to this case where we have 2 vertices (the start and end) and then 
 * build the viewport.
 */
void A2::resizeViewport(double xPos, double yPos) {
	if (storeViewportCorner) {
		selected_viewport_x = xPos;
		selected_viewport_y = yPos;
		viewport_x_vl = xPos;
		viewport_y_vb = yPos;
		storeViewportCorner = false;
	}
	if (ImGui::IsMouseDown(0)) {
		// Save xPos and yPos on click 
		// Constantly track xPos and yPos while holding 
		// Update viewport according to the xPos and yPos supplied while mouse is held down
		viewport_x_vl = std::min(xPos, double(viewport_x_vl));
		viewport_y_vb = std::min(yPos, double(viewport_y_vb));
		cout << viewport_x_vl << endl;
		cout << viewport_y_vb << endl;
		viewportWidth = abs(selected_viewport_x - xPos) * m_framebufferWidth/ 2;
		viewportHeight = abs(selected_viewport_y - yPos) * m_framebufferHeight / 2;
		// calculate each corner and find the one with lowest x and y, thats the viewport_x_vl etc?

	}
}

bool A2::clip(vec4 & v1, vec4 & v2) {
	// for each boundary
	for (int i = 0; i < 6; ++i) {
		float bound1 [6] = {
			v1.w + v1.x, // left
			v1.w - v1.x, // right
			v1.w + v1.y, // bottom
			v1.w - v1.y, // top
			v1.w + v1.z, // far
			v1.w - v1.z // near
		};

		float bound2 [6] = {
			v2.w + v2.x, // left
			v2.w - v2.x, // right
			v2.w + v2.y, // bottom
			v2.w - v2.y, // top
			v2.w + v2.z, // far
			v2.w - v2.z // near
		};

		if (bound1[i] < 0 && bound2[i] < 0) return false;
		if (bound1[i] >= 0 && bound2[i] >= 0) continue;

		float t = bound1[i] / (bound1[i] - bound2[i]);
		if (bound1[i] < 0)
			v1 = v1 + t * (v2 - v1);
		else
			v2 = v1 + t * (v2 - v1);
	}
	return true;
}
