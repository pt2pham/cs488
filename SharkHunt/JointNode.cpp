// Fall 2019

#include "JointNode.hpp"
using namespace glm;
using namespace std;
//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;

	curr_rot_x = init;
	rotate('x', init);
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;

	curr_rot_y = init;
	rotate('y', init);
}

void JointNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	// Ensure we don't rotate past the min and max defined in the .lua script
	double theta;
	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			theta = glm::clamp(
				(double) angle,
				m_joint_x.min - curr_rot_x, 
				m_joint_x.max - curr_rot_x
			);
			curr_rot_x += theta;
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
			theta = glm::clamp(
				(double) angle,
				m_joint_y.min - curr_rot_y, 
				m_joint_y.max - curr_rot_y
			);
			curr_rot_y += theta;
	        break;
		default:
			break;
	}

	mat4 rot_matrix = glm::rotate(degreesToRadians((float)theta), rot_axis);
	trans = rot_matrix * trans;
}
