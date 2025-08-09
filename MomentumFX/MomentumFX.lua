local MomentumFX = {}
MomentumFX.ActiveAnimations = {}
local Helper = require("Helper")
local Solver = require("Solver")

function MomentumFX.CreateAnimation(config,AnimationId)
    -- Validate required fields
    assert(type(config) == "table", "MomentumFX.CreateAnimation: config must be a table")
    assert(type(config.Parent) == "table", "MomentumFX.CreateAnimation: 'Parent' must be a table reference")
    assert(type(config.EasingFunc) == "function", "MomentumFX.CreateAnimation: 'EasingFunc' must be a function")

    local TableOutput = {
        Current = config.Current or 0,
        Goal = config.Goal or 0,
        Speed = config.Speed or 0,
        EasingFunc = config.EasingFunc,
        EventBindings = config.EventBindings or {},
        DefaultProperties = config.DefaultProperties or {},
        GoalProperties = config.GoalProperties or {},
        Parent = config.Parent,
        Params = config.Params or {},
        Paused = false,
    }
    MomentumFX.ActiveAnimations[AnimationId] = TableOutput;
    -- Return the animation object
    return TableOutput
end

function MomentumFX.Animate(Parent,EasingFunc,Goal,Params,AnimationId)
    return MomentumFX.CreateAnimation({
        Parent = Parent,
        Params = Params,
        DefaultProperties = Helper.DeepCopy(Parent),
        GoalProperties = Goal,
        Goal = 1,
        Current = 0,
        EasingFunc = EasingFunc,    
        IsBinded = false,
        BindedAnimation = nil,
        BindedRange = {FM = 0,FA = 0,TM = 0,TA = 0},
        EventBindings = {},
        BoundMin = 0,
        BoundMax = 1
    },AnimationId)
end

function MomentumFX.BindEvent(Animation, EventTrigger, EventFunc , EventId)
    Animation.EventBindings[EventId] = {Trigger = EventTrigger,Event = EventFunc}
    return Animation
end

function MomentumFX.UnbindEvent(Animation, EventId)
    Animation.EventBindings[EventId] = nil
    return Animation
end

function MomentumFX.BindToOtherAnimation(Animation, OtherAnimation, EffectiveMin, EffectiveMax, MappedMin, MappedMax)
    Animation.IsBinded = true
    Animation.BindedAnimation = OtherAnimation
    Animation.BindedRange = {FM = EffectiveMin,FA = EffectiveMax,TM = MappedMin,TA = MappedMax}
    return Animation
end
    

function MomentumFX.SetGoal(AnimationID, Goal)
    MomentumFX.ActiveAnimations[AnimationID].Goal = Goal
    return MomentumFX.ActiveAnimations[AnimationID]
end

function MomentumFX.PauseAnimation(Animation)
    Animation.Paused = true
    return Animation
end

function MomentumFX.ResumeAnimation(Animation)
    Animation.Paused = false
    return Animation    
end

function MomentumFX.DiscardAnimation(AnimationId)
    MomentumFX.ActiveAnimations[AnimationId] = nil
    return true
end

function MomentumFX.StepAnimation(Animation, DeltaTime , Time)
    if Animation.Paused then return end
    if Animation.IsBinded then
        local Binded = Animation.BindedAnimation
        local Effect = Binded.Current
        local Mapped = (Effect-Animation.BindedRange.FM)/(Animation.BindedRange.FA-Animation.BindedRange.FM)*(Animation.BindedRange.TA-Animation.BindedRange.TM)+Animation.BindedRange.TM
        Animation.Goal = Mapped
    end
                    
    Solver.Step(Animation,DeltaTime,Time)
    Helper.UpdateTable(Animation.Parent, Helper.Lerp(Animation.DefaultProperties, Animation.GoalProperties, Animation.Current))
    Helper.ForEachFunc(Animation.EventBindings, function(i, v)
        if v.Trigger(Animation) then
            v.Event(Animation)
         end
    end)
end

function MomentumFX.Update(DeltaTime,Time)
    Helper.ForEachFunc(MomentumFX.ActiveAnimations, function(i, v)
        MomentumFX.StepAnimation(v, DeltaTime,Time)
    end)
    return MomentumFX.ActiveAnimations
end

return MomentumFX
