
local Easing = {}
local helper = require("Helper")

--Returns Rate of change
function Easing.Linear(Goal,Current,Speed,DeltaTime,Params,Time)
    local Progress = (Time - Params.Start)/Params.Duration
    return Current - (Params.Start + (Goal - Params.Start) * Progress)
end

function Easing.DyanmicEaseOutExpo(Goal,Current,Speed,DeltaTime,Params,Time)
    local Speed = (Goal-Current)/(Params.Duration or 1.0)
    return Speed
end

function Easing.DynamicLinear(Goal,Current,Speed,DeltaTime,Params,Time)
    return (helper.Sign(Goal - Current)/Params.Duration)
end

function Easing.Set(Goal,Current,Speed,DeltaTime,Params,Time)
    return (Goal-Current)/DeltaTime
end

function Easing.EaseOutExpo(Goal, Current, Speed, DeltaTime, Params,Time)
    local t = DeltaTime / (Params.Duration or 1.0) -- Normalized time
    return Goal - (Goal - Current) * (2 ^ (-10 * t))
end

function Easing.Spring(Goal, Current, Speed, DeltaTime, Params, Time)
    return (Speed*Params.Damping) + (Goal - Current) * Params.Stiffness * DeltaTime
end

function Easing.Bounce(Goal, Current, Speed, DeltaTime, Params, Time)
    local Sign = helper.Sign(Goal - Current)
    if math.abs(Goal - Current) < Params.Threshold then
        return -Speed * Params.damping
    else
        return Speed + (Sign * Params.gravity * DeltaTime)
    end
end

return Easing

