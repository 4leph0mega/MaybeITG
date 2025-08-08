local win = Window("Test", 100, 100, 800, 600, false)
local tex = ImageBuffer()
tex:loadFromFile("example.jpg")
win:renderImageBuffer(tex)
win:setVSync(true)

function Update()
    win:renderImageBuffer(tex)
end
