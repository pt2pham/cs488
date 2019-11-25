// Fall 2019

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "Player.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <unordered_set>
#include <vector>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class SharkHunt : public CS488Window {
public:
	SharkHunt(const std::string & luaSceneFile);
	virtual ~SharkHunt();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();
	void initCamera();
	void loadTexture();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph();
	void renderArcCircle();

	void resetPosition();
	void resetOrientation();
	void resetJoints();
	void undo();
	void redo();
	void renderSkybox();

	void updatePosition(double diff);
	void updateJoints(double diff);

	void enableCulling();
	void disableCulling();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;
	bool do_picking;
	LightSource m_light;
	std::unordered_set<SceneNode *> selectedJointNodes;
	bool backface_culling;
	bool frontface_culling;
	bool zbuffer;
	bool circle;
	int transformation_mode;
	double old_x_pos;
	double old_y_pos;
	glm::mat4 init_root_trans;

	Player * player;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_uvCoords;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	SceneNode * m_rootNode;

	// Camera variables
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float cameraSpeed;

	bool firstMouseMove;
	float yaw;
	float pitch;
	float sensitivity;
};
