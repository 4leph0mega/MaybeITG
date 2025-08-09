local Helper = {}

    function Helper.Set(Table, Property, Value)
        Table[Property] = Value
        return Table
    end

    function Helper.Abs(x)
        return x < 0 and -x or x
    end

    function Helper.Pow(base, exp)
        local result = 1
        local absExp = Helper.Abs(exp)
        for i = 1, absExp do
            result = result * base
        end
        if exp < 0 then
             return 1 / result
        end
        return result
    end


    function Helper.ForEachFunc(Table, func)
        for i, v in pairs(Table) do
            func(i, v)
        end
    end

    function Helper.Lerp(TableA, TableB, Percentage)
        local NewTable = {}
            Helper.ForEachFunc(TableA, function(i, v)
                local b = TableB[i]
                if type(v) == "number" and type(b) == "number" then
                    NewTable[i] = v + (b - v) * Percentage
                else
                    NewTable[i] = v -- fallback: copy the original value from TableA
                end
            end)
         return NewTable
    end

    function Helper.Sign(Value)
        return Value > 0 and 1 or Value < 0 and -1 or 0
    end
    function Helper.DeepCopy(orig, copies)
    copies = copies or {}
    if type(orig) ~= 'table' then
        return orig
    elseif copies[orig] then
        return copies[orig]
    end

    local copy = {}
    copies[orig] = copy
    for k, v in next, orig, nil do
        copy[Helper.DeepCopy(k, copies)] = Helper.DeepCopy(v, copies)
    end
    setmetatable(copy, Helper.DeepCopy(getmetatable(orig), copies))
    return copy
end

function Helper.UpdateTable(dest, src)
    for k, v in pairs(src) do
        if dest[k] ~= nil then
            dest[k] = v
        end
    end
end

function Helper.Max(A,B)
    if A == nil then A = 0 end
    if B == nil then B = 0 end
    return A > B and A or B
end

function Helper.Min(A,B)
    if A == nil then A = 0 end
    if B == nil then B = 0 end
    return A < B and A or B
end

function Helper.Clamp(Value,Min,Max)
    if Value == nil then Value = 0 end
    if Min == nil then Min = 0 end
    if Max == nil then Max = 1 end
    return Helper.Max(Min,Helper.Min(Value,Max))
end

return Helper
