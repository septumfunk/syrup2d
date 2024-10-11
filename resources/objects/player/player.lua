require("engine.keyboard")

return {
    name = "test",
    depth = 0.0,
    update = function(this)
        if is_key_pressed(key.minus) then
            this.depth = this.depth - 1
            io.write(this.depth .. " ")
        end
        if is_key_pressed(key.equal) then
            this.depth = this.depth + 1
            io.write(this.depth .. " ")
        end
    end,
    draw = function(this)
        draw_sprite("vap", -20, -20)
    end
}