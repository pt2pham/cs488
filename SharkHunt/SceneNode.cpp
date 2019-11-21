// Fall 2019

#include "SceneNode.hpp"

#include <iostream>
#include <sstream>

using namespace std;
using namespace glm;

// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;
bool SceneNode::do_picking = false;

//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	isSelected(false),
	m_nodeId(nodeInstanceCount++),
	parent(nullptr)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

SceneNode * SceneNode::find_node(const unsigned int id) {
  queue<SceneNode*> tree;
  tree.push(this);

  while (!tree.empty()) {
    SceneNode* parent = tree.front();
    tree.pop();

    for(SceneNode * child : parent->children) {
      tree.push(child);

      if (child->m_nodeId == id) { return child; }
    }
  }
  return nullptr;
}

SceneNode * SceneNode::find_node(string name) {
  queue<SceneNode*> tree;
  tree.push(this);

  while (!tree.empty()) {
    SceneNode* parent = tree.front();
    tree.pop();

    for(SceneNode * child : parent->children) {
      tree.push(child);

      if (child->m_name == name) { return child; }
    }
  }
  return nullptr;
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	child->parent = this;
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	child->parent = nullptr;
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::render(
        const ShaderProgram & m_shader,
        const glm::mat4 & m_view,
        BatchInfoMap & m_batchInfoMap, 
        std::deque<glm::mat4> & stack 	
) 
{
	stack.push_back(trans);
	for (auto node : children) // Traverse tree from Root Node
		node->render(m_shader, m_view, m_batchInfoMap, stack);
	stack.pop_back();
}

void SceneNode::picking_on() {
	do_picking = true;
}

void SceneNode::picking_off() {
	do_picking = false;
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = rot_matrix * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}
