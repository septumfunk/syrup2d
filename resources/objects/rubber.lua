require("ui.info")
require("engine.mathutil")

return {
    depth = 5,
    size = { width = 10, height = 10 },
    velocity = { x = 0, y = 0 },
    gravity = 100,
    speed = 10,
    damping = 50,
    hovering = false,

    update = function(this)
        this.hovering = mouse.x > this.x
        and mouse.x < this.x + this.size.width
        and mouse.y > this.y
        and mouse.y < this.y + this.size.height

        if this.offset ~= nil then
            local dx = ((mouse.x - this.offset.x) - this.x)
            local dy = ((mouse.y - this.offset.y) - this.y)
            this.velocity.x = this.speed * dx
            this.velocity.y = this.speed * -dy
            this.x = this.x + this.velocity.x * delta_time
            this.y = this.y - this.velocity.y * delta_time

            if not is_mouse_button_down(mouse_button.left) then
                this.offset = nil
            end
            return
        end

        if this.hovering then
            if is_mouse_button_pressed(mouse_button.left) then
                this.offset = { x = mouse.x - this.x, y = mouse.y - this.y }
                return
            end
            if is_mouse_button_pressed(mouse_button.right) then
                object_delete(this.id)
                return
            end
        end

        this.x = this.x + this.velocity.x * delta_time
        this.y = this.y - this.velocity.y * delta_time

        -- Bottom & Gravity
        if this.y < dimensions.height - this.size.height then
            this.velocity.y = this.velocity.y - this.gravity * delta_time
        else
            this.y = dimensions.height - this.size.height
            this.velocity.y = -(this.velocity.y / 2.5)
            if this.velocity.y < 0.1 then
                this.velocity.y = 0
            end
            if this.velocity.y == 0 then
                this.velocity.x = approach(this.velocity.x, 0, this.damping * delta_time)
            end
        end

        -- Top
        if this.y < 0 then
            this.y = 0
            if this.velocity.y > 0 then
                this.velocity.y = -(this.velocity.y / 2.5)
            end
        end

        -- Left
        if this.x < 0 then
            this.x = 0
            if this.velocity.x < 0 then
                this.velocity.x = -(this.velocity.x / 2.5)
            end
        end

        -- Right
        if this.x > dimensions.width - this.size.width then
            this.x = dimensions.width - this.size.width
            if this.velocity.x > 0 then
                this.velocity.x = -(this.velocity.x / 2.5)
            end
        end
    end,

    draw_gui = function(this)
        draw_rectangle(this.x, this.y, this.size.width, this.size.height, ui_red)
    end,
}