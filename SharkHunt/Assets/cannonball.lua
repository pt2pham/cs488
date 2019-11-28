-- Cannonball

-- Define my colours
blue = gr.material({0.0, 0.5, 1.0}, {0.1, 0.1, 0.1}, 10)
dark_grey = gr.material({0.3, 0.3, 0.3}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

-- Create the top level root node named 'root'.
scene = gr.node('root')

ball = gr.mesh('cube', 'cube.obj')
scene:add_child(ball)
ball:set_material(white)
-- ball:scale(1.5, 1.5, 1.5)
-- ball = gr.mesh('Bomb', 'Bomb.obj')
-- scene:add_child(ball)
-- ball:set_material(dark_grey)
-- ball:scale(1.5, 1.5, 1.5)
--
-- Return the root with all of it's childern.
-- scene:translate(0, -1, 1)
return scene
