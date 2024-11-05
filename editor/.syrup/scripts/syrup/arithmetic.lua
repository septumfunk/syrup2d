function approach(start, target, step)
    if start < target then
        return math.min(start + step, target);
    else
        return math.max(start - step, target);
    end
end