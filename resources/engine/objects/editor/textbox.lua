require("syrup")

return {
    extends = "editor/control",
    selected = false,
    default = "...",
    text = "",
    limit = 25,
    border = 1,
    padding = 2,

    start = function(self)
        self:base_start()
        self.width = syrup.ui.font_size.width * math.max(self.limit, string.len(self.default)) + self.padding * 2 + self.border * 2
        self.height = syrup.ui.font_size.height + self.padding * 2 + self.border * 2
    end,

    update = function(self)
        self.width = syrup.ui.font_size.width * math.max(self.limit, string.len(self.default)) + self.padding * 2 + self.border * 2
        self.height = syrup.ui.font_size.height + self.padding * 2 + self.border * 2
        self:base_update()
        if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) then
            self.selected = self.hovering
            syrup.input.keyboard_string()
        end
        if self.selected then
            self.text = string.sub(self.text .. syrup.input.keyboard_string(), 0, self.limit)
            if syrup.input.is_key_pressed(syrup.input.key.backspace) then
                self.text = string.sub(self.text, 0, string.len(self.text) - 1)
            end
        end
    end,

    draw_ui = function(self)
        local x = self.x
        local y = self.y
        if self.centered then -- Why are you so self centered?
            x = self.x - self.width / 2
            y = self.y - self.height / 2
        end
        if self.selected then
            syrup.graphics.draw_rectangle(x, y, self.width, self.height, syrup.ui.black)
            syrup.graphics.draw_rectangle(x + self.border, y + self.border, self.width - self.border, self.height - self.border, syrup.ui.white)
        else
            syrup.graphics.draw_rectangle(x, y, self.width, self.height, syrup.ui.white)
            syrup.graphics.draw_rectangle(x + self.border, y + self.border, self.width - self.border, self.height - self.border, syrup.ui.black)
        end
        syrup.graphics.draw_rectangle(x + self.border, y + self.border, self.width - self.border * 2, self.height - self.border * 2, syrup.ui.secondary)
        if string.len(self.text) > 0 then
            syrup.graphics.draw_text("ui_font", x + self.border + self.padding, y + self.border + self.padding, self.text, syrup.ui.black)
            return
        end
        syrup.graphics.draw_text("ui_font", x + self.border + self.padding, y + self.border + self.padding, self.default, syrup.ui.black)
    end,
}