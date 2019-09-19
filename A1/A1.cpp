// Fall 2019

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
	// Grid
	colour[0][0] = 0.0f;
	colour[0][1] = 0.0f;
	colour[0][2] = 0.0f;
	// Cubes
	colour[1][0] = 0.5f;
	colour[1][1] = 0.0f;
	colour[1][2] = 0.0f;
	// Avatar
	colour[2][0] = 0.0f;
	colour[2][1] = 0.0f;
	colour[2][2] = 0.5f;
	model_rotation = 0.0f;
	model_scale = 1.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	m = make_shared<Maze>(DIM);
	
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;
	
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();
	avatar = make_shared<Cube>(0,1,0);

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

/*
 * 
 * CUBE OPERATIONS FOR A1
 * 
 * initCubeGrid
 * growWalls
 * shrinkWalls
 * initAvatar
 */
void A1::initCubeGrid(){
	int height = 1;
	for (int x = 0; x < DIM; x++) {
		vector<shared_ptr<Cube>> cube_col; 
		for (int z = 0; z < DIM; z++) {
			if (m->getValue(x, z) == 1) cube_col.push_back(make_shared<Cube>(x, height, z));
		}	
		cube_grid.push_back(cube_col);
	}
}

void A1::initAvatar(){
	// Find a viable starting position
	// Top border - [0][0...DIM]
	for (int z = 0; z < DIM; z++) {
		if (m->getValue(0, z) == 0) {
			avatar = make_shared<Cube>(0, 1, z);
			return;
		}
	}
	// Left border
	for (int x = 0; x < DIM; x++) {
		if (m->getValue(x, 0) == 0) {
			avatar = make_shared<Cube>(x, 1, 0);
			return;
		}
	}

	// Bottom border
	for (int z = 0; z < DIM; z++) {
		if (m->getValue(DIM, z) == 0) {
			avatar = make_shared<Cube>(DIM, 1, z);
			return;
		}
	}

	// Right border
	for (int x = 0; x < DIM; x++) {
		if (m->getValue(x, DIM) == 0) {
			avatar = make_shared<Cube>(x, 1, DIM);
			return;
		}
	}
}

void A1::growWalls() {
	for (auto col : cube_grid) 
		for (auto cube : col) 
			cube->growHeight(1.0f);
}

void A1::shrinkWalls() {
	for (auto col : cube_grid)
		for (auto cube : col)
			cube->shrinkHeight(1.0f);
}

void A1::moveAvatar(short int x, short int z, bool shiftEnabled) {
	unsigned short int nextXPos = avatar->xPos + x;
	unsigned short int nextZPos = avatar->zPos + z;

	if (shiftEnabled && m->getValue(nextXPos, nextZPos) == 1) {
		m->setValue(nextXPos, nextZPos, 0);
		// Disable cube itself to prevent drawing it
		for (auto col : cube_grid) { 
			for (auto cube : col) {
				if (cube->xPos == nextXPos && cube->zPos == nextZPos) {
					cube->disabled = true;
					avatar->translate(x, z);
					return;
				}
			}
		}
	} else if (m->getValue(nextXPos, nextZPos) == 0)
		avatar->translate(x, z);
}

void A1::reset() 
{
	// TODO: Reset state
	// There should be a Reset button that restores the grid to its initial, 
	// empty state, resets the view to the default, resets the colour 
	// to the initial colours, and moves the avatar back to the cell (0,0).
	
	// Grid
	colour[0][0] = 1.0f;
	colour[0][1] = 1.0f;
	colour[0][2] = 1.0f;
	// Cubes
	colour[1][0] = 1.0f;
	colour[1][1] = 1.0f;
	colour[1][2] = 1.0f;
	// Avatar
	colour[2][0] = 1.0f;
	colour[2][1] = 1.0f;
	colour[2][2] = 1.0f;

	model_rotation = 0.0f;
	model_scale = 1.0f;

	old_x_pos = 0.0;
	m->reset();
	cube_grid.clear();
	avatar = make_shared<Cube>(0,1,0);
}

void A1::dig() {
	m->digMaze();
	initAvatar();
	cube_grid.clear();
	initCubeGrid();
}
//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		if ( ImGui::Button( "Reset" ) ) { 
			reset();
		}

		if ( ImGui::Button( "Dig" ) ) {
			dig();
		}
		
		ImGui::ColorEdit3( "##Colour", colour[current_col] );
		ImGui::RadioButton( "Grid", &current_col, 0 );
		ImGui::RadioButton( "Maze", &current_col, 1 );
		ImGui::RadioButton( "Avatar", &current_col, 2 );
/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	W = glm::rotate(W, model_rotation, glm::vec3(0, 1, 0));
	W = glm::scale(W, vec3(model_scale));

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, colour[0][0], colour[0][1], colour[0][2]);
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		for (auto col : cube_grid) {
			for (auto cube : col) {
				glUniform3f( col_uni, colour[1][0], colour[1][1], colour[1][2] );
				if (!cube->disabled)
					cube->draw();
			}
		}
		// Highlight the active square.
		glUniform3f( col_uni, colour[2][0], colour[2][1], colour[2][2]);
		avatar->draw();
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		if (ImGui::IsMouseDown(0)) {
			float difference = xPos - old_x_pos;
			model_rotation += difference * 0.005f;
		}
		old_x_pos = xPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.
	if (yOffSet > 0) {
		model_scale *= 1 + (0.1 * yOffSet);
	} else {
		model_scale /= 1 - (0.1 * yOffSet);
	}
	model_scale = glm::clamp(model_scale, 0.0f, 10.0f);

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		switch (key) {
			// Quit
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(m_window, GL_TRUE);
				break;
			// Reset
			case GLFW_KEY_R:
				reset();
				eventHandled = true;
				break;
			// Dig
			case GLFW_KEY_D:
				dig();
				eventHandled = true;
				break;
			// Wall Height Change
			case GLFW_KEY_SPACE:
				growWalls();
				eventHandled = true;
				break;
			case GLFW_KEY_BACKSPACE:
				shrinkWalls();
				eventHandled = true;
				break;
			// Movements
			case GLFW_KEY_UP:
				moveAvatar(0, -1, mods == GLFW_MOD_SHIFT);
				eventHandled = true;
				break;
			case GLFW_KEY_DOWN:
				moveAvatar(0, 1, mods == GLFW_MOD_SHIFT);
				eventHandled = true;
				break;
			case GLFW_KEY_LEFT:
				moveAvatar(-1, 0, mods == GLFW_MOD_SHIFT);
				eventHandled = true;
				break;
			case GLFW_KEY_RIGHT:
				moveAvatar(1, 0, mods == GLFW_MOD_SHIFT);
				eventHandled = true;
				break;
			default:
				break;
		}
	}

	return eventHandled;
}
