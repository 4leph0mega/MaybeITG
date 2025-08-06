-- Create a borderless window at (200,200), size 400x300
win = Window.new("Lua Window", 200, 200, 400, 300, true)

-- Set background color to teal
win:set_color(0, 128, 128)

-- Clear (fill background)
win:clear()
