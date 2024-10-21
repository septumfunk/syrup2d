require("syrup")

return {
    extends = "editor/window",

    min_width = 250,
    min_height = 150,

    start = function(self)
        self:base_start()
        self:set_title("Edit Game Data")

        self.title_input = syrup.objects.new("editor/textbox", -999, -999)
        self.title_input.centered = true
        self.title_input.default = "Title..."
        self.min_width = self.title_input.width + 20

        self.width_input = syrup.objects.new("editor/textbox", -999, -999)
        self.width_input.centered = true
        self.width_input.limit = 5
        self.width_input.default = "Width"

        self.height_input = syrup.objects.new("editor/textbox", -999, -999)
        self.height_input.centered = true
        self.height_input.limit = 5
        self.height_input.default = "Height"

        self.scale_input = syrup.objects.new("editor/textbox", -999, -999)
        self.scale_input.centered = true
        self.scale_input.limit = 5
        self.scale_input.default = "Scale"

        self.save_button = syrup.objects.new("editor/button", -999, -999)
        self.save_button.centered = true
        self.save_button:set_text("Save and Exit")
        self.save_button.on_click = function(_)
            self:save_()
        end
    end,

    update = function(self)
        self:base_update()

        self.title_input.x = self.x + self.width / 2
        self.title_input.y = self.y + self.height / 5
        self.title_input:stick(self)

        self.width_input.x = self.x + self.width / 2 - self.width_input.width / 2 - ui_text_size.width
        self.width_input.y = self.y + self.height / 3
        self.width_input:stick(self)

        self.height_input.x = self.x + self.width / 2 + self.height_input.width / 2 + ui_text_size.width
        self.height_input.y = self.y + self.height / 3
        self.height_input:stick(self)

        self.scale_input.x = self.x + self.width / 2
        self.scale_input.y = self.y + self.height / 2
        self.scale_input:stick(self)

        self.save_button.x = self.x + self.width / 2
        self.save_button.y = self.y + self.height - self.height / 6
        self.save_button:stick(self)
    end,

    draw_ui = function(self)
        self:base_draw_ui()
        syrup.graphics.draw_text("ui_font", self.x + self.width / 2 - ui_text_size.width / 2, self.width_input.y - ui_text_size.height / 2, "x", ui_color_black)
    end,

    clean_up = function(self)
        self:base_clean_up()
        syrup.objects.delete(self.title_input.id)
        syrup.objects.delete(self.width_input.id)
        syrup.objects.delete(self.height_input.id)
        syrup.objects.delete(self.scale_input.id)
        syrup.objects.delete(self.save_button.id)
    end,

    spawn_object = function(self)
        if string.len(self.title_input.text) == 0 or string.len(self.width_input.text) == 0 or string.len(self.height_input.text) == 0 then
            return
        end

        local x = tonumber(self.width_input.text)
        if not x then
            self.width_input.text = ""
            return
        end

        local y = tonumber(self.height_input.text)
        if not y then
            self.height_input.text = ""
            return
        end

        local object = syrup.objects.new(self.title_input.text, x, y)
        if not object then
            self.title_input.text = ""
            return
        end
    end,
}