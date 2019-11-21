// Fall 2019

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};

	void rotate(char axis, float angle);

	float curr_rot_x;
	float curr_rot_y;

	JointRange m_joint_x, m_joint_y;
};
