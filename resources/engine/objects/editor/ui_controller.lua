require("syrup")

return {
    depth = -999,
    taskbar_padding = 1,

    start = function(self)
        self.wallpaper = syrup.objects.new("editor/wallpaper", 0, 0)

        self.cmd_button = syrup.objects.new("editor/button", 0, 0)
        self.cmd_button:set_text("Command")
        self.cmd_button.x = self.taskbar_padding
        self.cmd_button.y = syrup.dimensions.height - self.cmd_button.height - self.taskbar_padding
        self.cmd_button:stick(self)
        self.cmd_button.on_click = function(button)
            if self.cmd_button.command ~= nil then
                syrup.objects.delete(self.cmd_button.command.id)
                self.cmd_button.command = nil
                self.cmd_button.stick_down = false
                return
            end

            self.cmd_button.command = syrup.objects.new("editor/command", button.x - self.taskbar_padding, button.y - button.height - self.taskbar_padding * 2 - 1)
            self.cmd_button.stick_down = true
            self.cmd_button.command.o_clean_up = self.cmd_button.command.clean_up
            self.cmd_button.command.clean_up = function(_)
                self.cmd_button.command:o_clean_up()
                self.cmd_button.stick_down = false
                self.cmd_button.command = nil
            end
            if self.cmd_button.command == nil then return end
            self.cmd_button.command:add_entry("Spawn Basic Window", function(_)
                local window = syrup.objects.new("editor/window", 0, 0)
                if window == nil then return end

                window:set_title("Basic Window")
                window.x = syrup.dimensions.width / 2 - window.width / 2
                window.y = syrup.dimensions.height / 2 - window.height / 2
                syrup.objects.delete(self.cmd_button.command.id)
            end)
            self.cmd_button.command:add_entry("Spawn Object", function(_)
                local window = syrup.objects.new("editor/spawner", 0, 0)
                if window == nil then return end
                window.x = syrup.dimensions.width / 2 - window.width / 2
                window.y = syrup.dimensions.height / 2 - window.height / 2
                syrup.objects.delete(self.cmd_button.command.id)
            end)
        end
    end,

    draw_ui = function(self)
        syrup.graphics.draw_rectangle(
            0,
            syrup.dimensions.height - self.cmd_button.height - self.taskbar_padding * 2 - 2,
            syrup.dimensions.width,
            self.cmd_button.height + self.taskbar_padding * 2,
            ui_color_primary
        )
        syrup.graphics.draw_rectangle(
            0,
            syrup.dimensions.height - self.cmd_button.height - self.taskbar_padding * 2 - 1,
            syrup.dimensions.width,
            self.cmd_button.height + self.taskbar_padding * 2,
            ui_color_white
        )
        syrup.graphics.draw_rectangle(
            0,
            syrup.dimensions.height - self.cmd_button.height - self.taskbar_padding * 2,
            syrup.dimensions.width,
            self.cmd_button.height + self.taskbar_padding * 2,
            ui_color_primary
        )
    end,
}