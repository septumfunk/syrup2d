require("engine.keyboard")

return {
    name = "test",
    depth = 0.0,
    update = function(this)
        if is_key_down(key.w) then
            this.y = this.y - 1
        end
        if is_key_down(key.a) then
            this.x = this.x - 1
        end
        if is_key_down(key.s) then
            this.y = this.y + 1
        end
        if is_key_down(key.d) then
            this.x = this.x + 1
        end

        camera_center(this.x + sprite_width("vap") / 2, this.y + sprite_height("vap") / 2)
    end,
    draw = function(this)
        --io.write(this.x .. " " .. this.y)
        draw_sprite("vap", this.x, this.y)
    end
}