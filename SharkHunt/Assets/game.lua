-- Puppet

-- Define my colours
red = gr.material({1.0, 0.5, 0.5}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.5, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.5}, {0.1, 0.1, 0.1}, 10)
dark_grey = gr.material({0.3, 0.3, 0.3}, {0.1, 0.1, 0.1}, 10)
water = gr.material({176.0/255.0, 224.0/255.0, 230.0/255.0}, {0.1, 0.1, 0.1}, 10)

-- Create the top level root node named 'root'.
scene = gr.node('root')

cannon = gr.mesh('Proto_PortableCannon', 'Weapon_PortableCannon.obj')
scene:add_child(cannon)
cannon:set_material(dark_grey)
cannon:rotate('y', 180)
cannon:translate(0, 0, -7)

cannon1 = gr.mesh('Proto_PortableCannon', 'Weapon_PortableCannon.obj')
scene:add_child(cannon1)
cannon1:set_material(dark_grey)
cannon1:rotate('y', 180)
cannon1:translate(2, 0, -7)

cannon2 = gr.mesh('Proto_PortableCannon', 'Weapon_PortableCannon.obj')
scene:add_child(cannon2)
cannon2:set_material(dark_grey)
cannon2:rotate('y', 180)
cannon2:translate(-2, 0, -7)
--
-- Return the root with all of it's childern.
scene:scale(0.5, 0.5, 0.5)
return scene
