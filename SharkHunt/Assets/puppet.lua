-- Puppet

-- Define my colours
red = gr.material({1.0, 0.5, 0.5}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.5, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.5}, {0.1, 0.1, 0.1}, 10)
dark_grey = gr.material({0.1, 0.1, 0.1}, {0.1, 0.1, 0.1}, 10)
beige = gr.material({255.0/255, 205.0/255, 148.0/255}, {0.0, 0.0, 0.0}, 10)

torsoDim = {0.4, 1, 0.3}
shouldersDim = {0.6, 0.2, 0.3}
hipsDim = {0.45, 0.25, 0.3}
neckDim = {0.1, 0.15, 0.1}
upperArmDim = {0.15, 0.40, 0.15}
forearmDim = {0.10, 0.35, 0.10}
headDim = {0.3, 0.3, 0.3}
handDim = {0.1, 0.1, 0.05}
thighDim = {0.25, 0.45, 0.25}
calfDim = {0.20, 0.45, 0.20}
footDim = {0.1, 0.1, 0.35}

-- Create the top level root node named 'root'.
rootNode = gr.node('root')
rootNode:rotate('y', 180)

-- 
torso = gr.mesh('sphere', 'torso')
rootNode:add_child(torso)
torso:scale(torsoDim[1], torsoDim[2], torsoDim[3])
torso:translate(0, 0.5, 0)
torso:set_material(beige)

-- LEVEL 1

--TORSO->SHOULDERS
shoulders = gr.mesh('sphere', 'shoulders')
torso:add_child(shoulders)
shoulders:scale(1 / torsoDim[1], 1 / torsoDim[2], 1 / torsoDim[3])
shoulders:scale(shouldersDim[1], shouldersDim[2], shouldersDim[3])
shoulders:translate(0, 0.85, 0)
shoulders:set_material(beige)

joint_neck = gr.joint('shoulder-neck-joint', {-45, 0, 45}, {0, 0, 0})
joint_left_arm = gr.joint('shoulder-left-arm-joint', {-180, 0, 45}, {0, 0, 0})
joint_right_arm = gr.joint('shoulder-right-arm-joint', {-180, 0, 45}, {0, 0, 0})
shoulders:add_child(joint_neck) -- Add joint to connect neck
shoulders:add_child(joint_left_arm) -- Add joint to connect arm
shoulders:add_child(joint_right_arm) -- Add joint to connect arm



-- TORSO->SHOULDERS->NECK
neck = gr.mesh('sphere', 'neck')
joint_neck:add_child(neck)
neck:scale(1 / shouldersDim[1], 1 / shouldersDim[2], 1 / shouldersDim[3])
neck:scale(neckDim[1], neckDim[2], neckDim[3]) -- Scaled to smaller version of torso
neck:translate(0, 1.0, 0)
neck:set_material(beige)

joint_head = gr.joint('neck-head-joint', {-15, 0, 15}, {-90, 0, 90}) -- Add joint to connect head
neck:add_child(joint_head)



-- TORSO->SHOULDERS->NECK->HEAD
head = gr.mesh('sphere', 'head')
joint_head:add_child(head)
head:scale(1 / neckDim[1], 1 / neckDim[2], 1 / neckDim[3])
head:scale(headDim[1], headDim[2], headDim[3]) -- Scaled to smaller version of torso
head:translate(0, 2.2, 0)
head:set_material(beige)
-- TORSO->SHOULDERS->NECK->HEAD->NOSE
nose = gr.mesh('sphere', 'nose')
head:add_child(nose)
nose:scale(1 / headDim[1], 1 / headDim[2], 1 / headDim[3])
nose:scale(0.03, 0.03, 0.4)
nose:translate(0, -0.2, 0.4)
nose:set_material(beige)



-- TORSO->SHOULDERS->UPPER ARMS
-- Mesh
leftUpperArm = gr.mesh('sphere', 'left-upper-arm')
joint_left_arm:add_child(leftUpperArm)
rightUpperArm = gr.mesh('sphere', 'right-upper-arm')
joint_right_arm:add_child(rightUpperArm)
-- Trans
leftUpperArm:scale(1 / shouldersDim[1], 1 / shouldersDim[2], 1 / shouldersDim[3]) 
leftUpperArm:scale(upperArmDim[1], upperArmDim[2], upperArmDim[3]) 
rightUpperArm:scale(1 / shouldersDim[1], 1 / shouldersDim[2], 1 / shouldersDim[3]) 
rightUpperArm:scale(upperArmDim[1], upperArmDim[2], upperArmDim[3]) 
leftUpperArm:translate(-0.9, -1.55, 0) 
rightUpperArm:translate(0.9, -1.55, 0) 
leftUpperArm:set_material(beige) 
rightUpperArm:set_material(beige) 
 -- Add joint to connect forearms
joint_left_elbow = gr.joint('left-elbow-joint', {-90, 0, 90}, {0, 0, 0})
joint_right_elbow = gr.joint('right-elbow-joint', {-90, 0, 90}, {0, 0, 0})
leftUpperArm:add_child(joint_left_elbow)
rightUpperArm:add_child(joint_right_elbow)



 -- TORSO->SHOULDERS->UPPER ARMS->FOREARMS
 -- Mesh
 leftForeArm = gr.mesh('sphere', 'left-fore-arm')
 joint_left_elbow:add_child(leftForeArm)
 rightForeArm = gr.mesh('sphere', 'right-fore-arm')
 joint_right_elbow:add_child(rightForeArm)
 -- Trans
 leftForeArm:scale(1 / upperArmDim[1], 1 / upperArmDim[2], 1 / upperArmDim[3]) 
 leftForeArm:scale(forearmDim[1], forearmDim[2], forearmDim[3]) 
 rightForeArm:scale(1 / upperArmDim[1], 1 / upperArmDim[2], 1 / upperArmDim[3]) 
 rightForeArm:scale(forearmDim[1], forearmDim[2], forearmDim[3]) 
 leftForeArm:translate(0, -1.5, 0) 
 rightForeArm:translate(0, -1.5, 0) 
 leftForeArm:set_material(beige) 
 rightForeArm:set_material(beige)

 -- Add joint to connect forearms
 joint_left_wrist = gr.joint('left-wrist-joint', {-45, 0, 45}, {0, 0, 0})
 joint_right_wrist = gr.joint('right-wrist-joint', {-45, 0, 45}, {0, 0, 0})
 leftForeArm:add_child(joint_left_wrist)
 rightForeArm:add_child(joint_right_wrist)



 -- TORSO->SHOULDERS->UPPER ARMS->FOREARMS->HANDS
 -- Mesh
 leftHand = gr.mesh('sphere', 'left-hand')
 joint_left_wrist:add_child(leftHand)
 rightHand = gr.mesh('sphere', 'right-hand')
 joint_right_wrist:add_child(rightHand)
 -- Trans
 leftHand:scale(1 / forearmDim[1], 1 / forearmDim[2], 1 / forearmDim[3]) 
 leftHand:scale(handDim[1], handDim[2], handDim[3]) 
 rightHand:scale(1 / forearmDim[1], 1 / forearmDim[2], 1 / forearmDim[3]) 
 rightHand:scale(handDim[1], handDim[2], handDim[3]) 
 leftHand:translate(0, -1.2, 0) 
 rightHand:translate(0, -1.2, 0) 
 leftHand:set_material(beige) 
 rightHand:set_material(beige) 



--TORSO->HIPS
hips = gr.mesh('sphere', 'hips')
torso:add_child(hips)
hips:scale(1 / torsoDim[1], 1 / torsoDim[2], 1 / torsoDim[3]) 
hips:scale(hipsDim[1], hipsDim[2], hipsDim[3]) 
hips:translate(0, -0.85, 0)
hips:set_material(red) 
-- Add joints to connect left and right thigh
joint_left_thigh = gr.joint('hips-left-thigh-joint', {-45, 0, 90}, {0, 0, 0})
joint_right_thigh = gr.joint('hips-right-thigh-joint', {-45, 0, 90}, {0, 0, 0})
hips:add_child(joint_left_thigh) -- Add joint to connect thigh
hips:add_child(joint_right_thigh) -- Add joint to connect thigh



-- TORSO->HIPS->THIGHS
leftThigh = gr.mesh('sphere', 'left-thigh')
joint_left_thigh:add_child(leftThigh)
rightThigh = gr.mesh('sphere', 'right-thigh')
joint_right_thigh:add_child(rightThigh)

leftThigh:scale(1 / hipsDim[1], 1 / hipsDim[2], 1 / hipsDim[3]) 
leftThigh:scale(thighDim[1], thighDim[2], thighDim[3]) 
rightThigh:scale(1 / hipsDim[1], 1 / hipsDim[2], 1 / hipsDim[3]) 
rightThigh:scale(thighDim[1], thighDim[2], thighDim[3]) 
leftThigh:translate(-0.7, -1.5, 0) 
rightThigh:translate(0.7, -1.5, 0) 
leftThigh:set_material(red) 
rightThigh:set_material(red) 
 -- Add joint to connect calves
joint_left_knee = gr.joint('left-knee', {-90, 0, 90}, {0, 0, 0})
joint_right_knee = gr.joint('right-knee', {-90, 0, 90}, {0, 0, 0})
leftThigh:add_child(joint_left_knee)
rightThigh:add_child(joint_right_knee)



-- TORSO->HIPS->THIGHS->CALVES
leftCalf = gr.mesh('sphere', 'left-calf')
joint_left_knee:add_child(leftCalf)
rightCalf = gr.mesh('sphere', 'right-calf')
joint_right_knee:add_child(rightCalf)

leftCalf:scale(1 / thighDim[1], 1 / thighDim[2], 1 / thighDim[3]) 
leftCalf:scale(calfDim[1], calfDim[2], calfDim[3]) 
rightCalf:scale(1 / thighDim[1], 1 / thighDim[2], 1 / thighDim[3]) 
rightCalf:scale(calfDim[1], calfDim[2], calfDim[3]) 
leftCalf:translate(0, -1.5, 0) 
rightCalf:translate(0, -1.5, 0) 
leftCalf:set_material(beige) 
rightCalf:set_material(beige) 
 -- Add joint to connect feet
joint_left_foot = gr.joint('left-foot-joint', {0, 0, 90}, {0, 0, 90})
joint_right_foot = gr.joint('right-foot-joint', {0, 0, 90}, {0, 0, 90})
leftCalf:add_child(joint_left_foot)
rightCalf:add_child(joint_right_foot)



-- TORSO->HIPS->THIGHS->CALVES->FEET
leftFoot = gr.mesh('sphere', 'left-foot')
joint_left_foot:add_child(leftFoot)
rightFoot = gr.mesh('sphere', 'right-foot')
joint_right_foot:add_child(rightFoot)

leftFoot:scale(1 / calfDim[1], 1 / calfDim[2], 1 / calfDim[3]) 
leftFoot:scale(footDim[1], footDim[2], footDim[3]) 
rightFoot:scale(1 / calfDim[1], 1 / calfDim[2], 1 / calfDim[3]) 
rightFoot:scale(footDim[1], footDim[2], footDim[3]) 
leftFoot:translate(0, -1, 0.5) 
rightFoot:translate(0, -1, 0.5) 
leftFoot:set_material(dark_grey) 
rightFoot:set_material(dark_grey) 
--
-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.

rootNode:translate(0, 0, -7)
return rootNode
