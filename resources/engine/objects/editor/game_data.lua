require("syrup")

return {
    extends = "editor/window",

    min_width = 250,
    min_height = 150,

    start = function(self)
        self:base_start()
        self:set_title("Edit Game Data")

        local data = syrup.resources.get_game_data()

        self.title_input = syrup.objects.new("editor/textbox", -999, -999)
        self.title_input.centered = true
        self.title_input.default = "Title..."
        self.min_width = self.title_input.width + 20
        self.title_input.text = data.title

        self.width_input = syrup.objects.new("editor/textbox", -999, -999)
        self.width_input.centered = true
        self.width_input.limit = 5
        self.width_input.default = "Width"
        self.width_input.text = tostring(data.width)

        self.height_input = syrup.objects.new("editor/textbox", -999, -999)
        self.height_input.centered = true
        self.height_input.limit = 5
        self.height_input.default = "Height"
        self.height_input.text = tostring(data.height)

        self.scale_input = syrup.objects.new("editor/textbox", -999, -999)
        self.scale_input.centered = true
        self.scale_input.limit = 5
        self.scale_input.default = "Scale"
        self.scale_input.text = tostring(data.window_scale)

        self.save_button = syrup.objects.new("editor/button", -999, -999)
        self.save_button.centered = true
        self.save_button:set_text("Save and Exit")
        self.save_button.on_click = function(_)
            self:save()
        end
    end,

    update = function(self)
        self:base_update()

        self.title_input.x = self.x + self.width / 2
        self.title_input.y = self.y + self.height / 5
        self.title_input:stick(self)

        self.width_input.x = self.x + self.width / 2 - self.width_input.width / 2 - syrup.ui.font_size.width
        self.width_input.y = self.y + self.height / 3
        self.width_input:stick(self)

        self.height_input.x = self.x + self.width / 2 + self.height_input.width / 2 + syrup.ui.font_size.width
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
        syrup.graphics.draw_text("ui_font", self.x + self.width / 2 - syrup.ui.font_size.width / 2, self.width_input.y - syrup.ui.font_size.height / 2, "x", syrup.ui.black)
    end,

    clean_up = function(self)
        self:base_clean_up()
        syrup.objects.delete(self.title_input.id)
        syrup.objects.delete(self.width_input.id)
        syrup.objects.delete(self.height_input.id)
        syrup.objects.delete(self.scale_input.id)
        syrup.objects.delete(self.save_button.id)
    end,

    save = function(self)
        if string.len(self.title_input.text) == 0 or string.len(self.width_input.text) == 0 or string.len(self.height_input.text) == 0 then
            return
        end

        local width = tonumber(self.width_input.text)
        if not width then
            self.width_input.text = ""
            return
        end

        local height = tonumber(self.height_input.text)
        if not height then
            self.height_input.text = ""
            return
        end

        local scale = tonumber(self.scale_input.text)
        if not scale then
            self.scale_input.text = ""
            return
        end

        syrup.resources.set_game_data(self.title_input.text, width, height, scale)
    end,
}