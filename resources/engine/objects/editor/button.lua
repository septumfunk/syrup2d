require("syrup")

return {
    extends = "editor/control",

    padding = 2,
    border = 1,
    held = false,
    stick_down = false,

    start = function(self)
        self:set_text("Default")
    end,

    update = function(self)
        self:base_update()
        if self.hovering and syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) then
            self.held = true
        end
        if self.held and not syrup.input.is_mouse_button_down(syrup.input.mouse_button.left) then
            self.held = false
            self:on_click()
        end
    end,

    draw_ui = function(self)
        local x = self.x
        local y = self.y
        if self.centered then
            x = x - self.width / 2
            y = y - self.height / 2
        end

        if self.stick_down or self.hovering then
            if self.hovering then
                syrup.input.set_cursor(syrup.input.cursor.pointer)
            end
            if self.stick_down or syrup.input.is_mouse_button_down(syrup.input.mouse_button.left) then
                syrup.graphics.draw_rectangle(x, y, self.width, self.height, ui_color_black)
                syrup.graphics.draw_rectangle(x + self.border, y + self.border, self.width - self.border, self.height - self.border, ui_color_white)
                syrup.graphics.draw_rectangle(x + self.border, y + self.border, self.width - self.border * 2, self.height - self.border * 2, ui_color_primary)
                syrup.graphics.draw_text("ui_font", x + self.padding + self.border, y + self.padding + self.border, self._text, ui_color_black)
                return
            end
        end
        syrup.graphics.draw_rectangle(x, y, self.width, self.height, ui_color_white)
        syrup.graphics.draw_rectangle(x + self.border, y + self.border, self.width - self.border, self.height - self.border, ui_color_black)
        syrup.graphics.draw_rectangle(x + self.border, y + self.border, self.width - self.border * 2, self.height - self.border * 2, ui_color_primary)
        syrup.graphics.draw_text("ui_font", x + self.border + self.padding, y + self.border + self.padding, self._text, ui_color_black)
    end,

    clean_up = function(self)
        if self.hovering then
            syrup.input.set_cursor(syrup.input.cursor.normal)
        end
    end,

    on_click = function(self)
        io.write(self._text .. " says: \"I've been clicked!\"\n")
    end,

    set_text = function(self, text)
        self._text = text
        self.width = string.len(text) * ui_text_size.width + self.padding * 2 + self.border * 2
        self.height = ui_text_size.height + self.padding * 2 + self.border * 2
    end,

    set_width = function(self, width)
        if not width then
            error("Uh oh!")
        end
        self.width = width + self.padding * 2 + self.border * 2
        self.height = ui_text_size.height + self.padding * 2 + self.border * 2
    end,
}