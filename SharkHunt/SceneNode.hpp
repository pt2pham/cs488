// Fall 2019

#pragma once

#include "Material.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <list>
#include <string>
#include <iostream>
#include <stack>
#include <queue>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

    SceneNode * find_node(const unsigned int id);
    SceneNode * find_node(std::string name);

    void picking_on();
    void picking_off();

    virtual void render(
        const ShaderProgram & m_shader,
        const glm::mat4 & m_view,
        BatchInfoMap & m_batchInfoMap, 
        std::deque<glm::mat4> & stack
    );

	//-- Transformations:
    virtual void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;
    static bool do_picking;
    SceneNode * parent;
    
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
