local Solver = {}
local Helper = require("Helper")

function Solver.Step(Animation, DeltaTime, Time)
    local Speed = Animation.EasingFunc(Animation.Goal, Animation.Current, Animation.Speed, DeltaTime, Animation.Params, Time)
    Animation.Speed = Speed
    Animation.Current = Animation.Current + Animation.Speed * DeltaTime
    Animation.Current = Helper.Clamp(Animation.Current,Animation.BoundMin,Animation.BoundMax)
    return Animation
end

return Solver

