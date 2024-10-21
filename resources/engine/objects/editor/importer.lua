require("syrup")

return {
    extends = "editor/window",

    start = function(self)
        self:base_start()
        self:set_title("Import Sprite")

        self.name_input = syrup.objects.new("editor/textbox", -999, -999)
        self.name_input.centered = true
        self.name_input.default = "Sprite Name..."
        self.min_width = self.name_input.width + 20

        self.frame_count_input = syrup.objects.new("editor/textbox", -999, -999)
        self.frame_count_input.centered = true
        self.frame_count_input.limit = 3
        self.frame_count_input.default = "Frames"

        self.frame_delay_input = syrup.objects.new("editor/textbox", -999, -999)
        self.frame_delay_input.centered = true
        self.frame_delay_input.limit = 3
        self.frame_delay_input.default = "Delay"

        self.spawn_button = syrup.objects.new("editor/button", -999, -999)
        self.spawn_button.centered = true
        self.spawn_button:set_text("Import")
        self.spawn_button.on_click = function(_)
            self:import_sprite()
        end
    end,

    update = function(self)
        self:base_update()

        self.name_input.x = self.x + self.width / 2
        self.name_input.y = self.y + self.height / 4
        self.name_input:stick(self)

        self.frame_count_input.x = self.x + self.width / 2 - self.frame_count_input.width / 2 - ui_text_size.width
        self.frame_count_input.y = self.y + self.height / 2
        self.frame_count_input:stick(self)

        self.frame_delay_input.x = self.x + self.width / 2 + self.frame_delay_input.width / 2 + ui_text_size.width
        self.frame_delay_input.y = self.y + self.height / 2
        self.frame_delay_input:stick(self)

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
        syrup.objects.delete(self.name_input.id)
        syrup.objects.delete(self.frame_count_input.id)
        syrup.objects.delete(self.frame_delay_input.id)
        syrup.objects.delete(self.spawn_button.id)
    end,

    import_sprite = function(self)
        if string.len(self.name_input.text) == 0 or string.len(self.frame_count_input.text) == 0 or string.len(self.frame_delay_input.text) == 0 then
            return
        end

        local frame_count = tonumber(self.frame_count_input.text)
        if not frame_count then
            self.frame_count_input.text = ""
            return
        end

        local frame_delay = tonumber(self.frame_delay_input.text)
        if not frame_delay then
            self.frame_delay_input.text = ""
            return
        end

        syrup.resources.import_sprite(self.name_input.text, frame_count, frame_delay)
        local v = syrup.objects.new("editor/viewer", self.x + 10, self.y + 10)
        v.sprite = self.name_input.text
        syrup.objects.delete(self.id)
    end,
}