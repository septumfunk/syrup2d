require("syrup")

return {
    extends = "editor/control",

    padding = 2,
    border = 1,
    held = false,
    stick_down = false,

    cursor = syrup.input.cursor.pointer,

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

        syrup.ui.draw_classic_box(x, y, self.width + self.border * 2, self.height + self.border * 2, self.held or self.stick_down)
        if self.stick_down or self.held then
            syrup.graphics.draw_text("ui_font", x + self.padding + self.border + 1, y + self.padding + self.border + 1, self._text, syrup.ui.black)
        else
            syrup.graphics.draw_text("ui_font", x + self.padding + self.border, y + self.padding + self.border, self._text, syrup.ui.black)
        end
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
        self.width = string.len(text) * syrup.ui.font_size.width + self.padding * 2 + self.border * 2
        self.height = syrup.ui.font_size.height + self.padding * 2 + self.border * 2
    end,

    set_width = function(self, width)
        if not width then
            error("Uh oh!")
        end
        self.width = width + self.padding * 2 + self.border * 2
        self.height = syrup.ui.font_size.height + self.padding * 2 + self.border * 2
    end,
}