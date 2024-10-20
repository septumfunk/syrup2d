require("syrup")

return {
    extends = "editor/window",

    start = function(self)
        self:base_start()
        self:set_title("Spawn Object")

        self.type_input = syrup.objects.new("editor/textbox", 0, 0)
        self.type_input.centered = true
        self.type_input.default = "Object Type..."
        self.min_width = self.type_input.width + 20

        self.x_input = syrup.objects.new("editor/textbox", 0, 0)
        self.x_input.centered = true
        self.x_input.limit = 5
        self.x_input.default = "X Pos"

        self.y_input = syrup.objects.new("editor/textbox", 0, 0)
        self.y_input.centered = true
        self.y_input.limit = 5
        self.y_input.default = "Y Pos"

        self.spawn_button = syrup.objects.new("editor/button", 0, 0)
        self.spawn_button.centered = true
        self.spawn_button:set_text("Spawn")
        self.spawn_button.on_click = function(_)
            self:spawn_object()
        end
    end,

    update = function(self)
        self:base_update()

        self.type_input.x = self.x + self.width / 2
        self.type_input.y = self.y + self.height / 4
        self.type_input:stick(self)

        self.x_input.x = self.x + self.width / 2 - self.x_input.width / 2 - ui_text_size.width
        self.x_input.y = self.y + self.height / 2
        self.x_input:stick(self)

        self.y_input.x = self.x + self.width / 2 + self.y_input.width / 2 + ui_text_size.width
        self.y_input.y = self.y + self.height / 2
        self.y_input:stick(self)

        self.spawn_button.x = self.x + self.width / 2
        self.spawn_button.y = self.y + self.height - self.height / 4
        self.spawn_button:stick(self)
    end,

    draw_ui = function(self)
        self:base_draw_ui()
        syrup.graphics.draw_text("ui_font", self.x + self.width / 2 - ui_text_size.width / 2, self.y + self.height / 2 - ui_text_size.height / 2, "x", ui_color_black)
    end,

    clean_up = function(self)
        self:base_clean_up()
        syrup.objects.delete(self.type_input.id)
        syrup.objects.delete(self.x_input.id)
        syrup.objects.delete(self.y_input.id)
        syrup.objects.delete(self.spawn_button.id)
    end,

    spawn_object = function(self)
        if string.len(self.type_input.text) == 0 or string.len(self.x_input.text) == 0 or string.len(self.y_input.text) == 0 then
            return
        end

        local x = tonumber(self.x_input.text)
        if not x then
            self.x_input.text = ""
            return
        end

        local y = tonumber(self.y_input.text)
        if not y then
            self.y_input.text = ""
            return
        end

        local object = syrup.objects.new(self.type_input.text, x, y)
        if not object then
            self.type_input.text = ""
            return
        end
    end,
}