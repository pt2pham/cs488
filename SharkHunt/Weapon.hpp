// Fall 2019

#pragma once

#include "Material.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "SceneNode.hpp"

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


class Weapon {
public:
    Weapon(const std::string & name);

	Weapon(const Weapon & other);

    virtual ~Weapon();
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;

    // TODO: Will probably load init Weapon with some lua script that 
    SceneNode * Weapon_model; 

    void set_transform(const glm::mat4& m);

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


	friend std::ostream & operator << (std::ostream & os, const Weapon & node);

	bool isSelected;
    static bool do_picking;
    Weapon * parent;
    
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<Weapon*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;
private:
	// The number of Weapon instances.
	static unsigned int nodeInstanceCount;
};
