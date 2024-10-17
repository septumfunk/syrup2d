function object()
    local table = {}
    table.__index = table
    return table
end

function copy_table(table)
    local new_table = {}
    for k,v in pairs(table) do
        new_table[k] = v
    end
    return new_table
end