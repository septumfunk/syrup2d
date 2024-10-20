require("syrup")

return {
    depth = 5,
    size = { width = 30, height = 30 },
    velocity = { x = 0, y = 0 },
    speed = 50,
    damping = 50,
    hovering = false,
    color = { r = 255, g = 125, b = 125, a = 255 },

    border = 1,

    update = function(self)
        self.hovering = syrup.mouse.x > self.x
        and syrup.mouse.x < self.x + self.size.width
        and syrup.mouse.y > self.y
        and syrup.mouse.y < self.y + self.size.height

        if self.offset ~= nil then
            local dx = ((syrup.mouse.x - self.offset.x) - self.x)
            local dy = ((syrup.mouse.y - self.offset.y) - self.y)
            self.velocity.x = self.speed * dx
            self.velocity.y = self.speed * -dy
            self.x = self.x + self.velocity.x * syrup.delta_time
            self.y = self.y - self.velocity.y * syrup.delta_time

            if not syrup.input.is_mouse_button_down(syrup.input.mouse_button.left) then
                self.offset = nil
            end
            return
        end

        if self.hovering then
            if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) then
                self.offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                return
            end
            if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.right) then
                syrup.objects.delete(self.id)
                return
            end
        end

        self.x = self.x + self.velocity.x * syrup.delta_time
        self.y = self.y - self.velocity.y * syrup.delta_time

        -- Bottom & Gravity
        if self.y > syrup.dimensions.height - self.size.height then
            self.y = syrup.dimensions.height - self.size.height
            if self.velocity.y < 0 then
                self.velocity.y = -(self.velocity.y / 1.5)
                self.color = { r = math.random(255), g = math.random(255), b = math.random(255), a = 255 }
            end
        end

        -- Top
        if self.y < 0 then
            self.y = 0
            if self.velocity.y > 0 then
                self.velocity.y = -(self.velocity.y / 1.5)
                self.color = { r = math.random(255), g = math.random(255), b = math.random(255), a = 255 }
            end
        end

        -- Left
        if self.x < 0 then
            self.x = 0
            if self.velocity.x < 0 then
                self.velocity.x = -(self.velocity.x / 1.5)
                self.color = { r = math.random(255), g = math.random(255), b = math.random(255), a = 255 }
            end
        end

        -- Right
        if self.x > syrup.dimensions.width - self.size.width then
            self.x = syrup.dimensions.width - self.size.width
            if self.velocity.x > 0 then
                self.velocity.x = -(self.velocity.x / 1.5)
                self.color = { r = math.random(255), g = math.random(255), b = math.random(255), a = 255 }
            end
        end

        self.velocity.x = approach(self.velocity.x, 0, self.damping * syrup.delta_time)
        self.velocity.y = approach(self.velocity.y, 0, self.damping * syrup.delta_time)
    end,

    draw_ui = function(self)
        if self.offset ~= nil then
            syrup.graphics.draw_rectangle(self.x, self.y, self.size.width, self.size.height, ui_color_black)
            syrup.graphics.draw_rectangle(self.x + self.border, self.y + self.border, self.size.width - self.border, self.size.height - self.border, ui_color_white)
        else
            syrup.graphics.draw_rectangle(self.x, self.y, self.size.width, self.size.height, ui_color_white)
            syrup.graphics.draw_rectangle(self.x + self.border, self.y + self.border, self.size.width - self.border, self.size.height - self.border, ui_color_black)
        end
        syrup.graphics.draw_rectangle(self.x + self.border, self.y + self.border, self.size.width - self.border * 2, self.size.height - self.border * 2, ui_color_primary)
    end,
}