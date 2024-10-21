require("syrup")

return {
    extends = "editor/window",

    min_width = 250,
    min_height = 200,

    sprite = "default",

    start = function(self)
        self:base_start()
        self:set_title("View Sprite")

        self.sprite_input = syrup.objects.new("editor/textbox", -999, -999)
        self.sprite_input.default = "Sprite Name..."
        self.sprite_input.centered = true

        self.view_button = syrup.objects.new("editor/button", -999, -999)
        self.view_button:set_text("View")
        self.view_button.centered = true
        self.view_button.on_click = function(_)
            if string.len(self.sprite) > 0 then
                self.sprite = self.sprite_input.text
                self.sprite_input.text = ""
            end
        end
    end,

    update = function(self)
        self:base_update()

        self.sprite_input.x = self.x + self.width - self.width / 2
        self.sprite_input.y = self.y + self.height - self.height / 4
        self.sprite_input:stick(self)

        self.view_button.x = self.x + self.width - self.width / 2
        self.view_button.y = self.y + self.height - self.height / 10
        self.view_button:stick(self)
    end,

    draw_ui = function(self)
        self:base_draw_ui()
        local dimensions = syrup.graphics.sprite_dimensions(self.sprite)

        local x_scale = 1
        local y_scale = 1
        if self.width <= self.height then
            x_scale = (self.width - 10 - self.width / 3) / dimensions.width
            y_scale = (self.width - 10 - self.width / 3) / dimensions.width
        else
            x_scale = (self.height - 10 - self.height / 3) / dimensions.height
            y_scale = (self.height - 10 - self.height / 3) / dimensions.height
        end

        syrup.graphics.draw_sprite_pro(self.sprite, self.x + self.width / 2 - dimensions.width * x_scale / 2, self.y + 5, x_scale, y_scale, -999, -999, ui_color_white)
    end,

    clean_up = function(self)
        self:base_clean_up()
        syrup.objects.delete(self.sprite_input.id)
        syrup.objects.delete(self.view_button.id)
    end,
}