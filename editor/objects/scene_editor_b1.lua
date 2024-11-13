require("syrup")

return {
    instances = {},

    pan = nil,
    cam = { x = 0, y = 0, z = 0 },

    start = function(self)
        self:load_instances()
        syrup.graphics.set_camera_center(0, 0)
    end,

    update = function(self)
        self.cam = syrup.graphics.get_camera_center()

        if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.middle) then
            syrup.input.set_cursor(syrup.input.cursor.crosshair)
            self.pan = {
                mouse = {
                    x = syrup.mouse.x,
                    y = syrup.mouse.y,
                },
                camera = syrup.graphics.get_camera_center(),
            }
        end

        if self.pan then
            syrup.graphics.set_camera_center(self.pan.camera.x - (syrup.mouse.x - self.pan.mouse.x), self.pan.camera.y - (syrup.mouse.y - self.pan.mouse.y))
        end

        if not syrup.input.is_mouse_button_down(syrup.input.mouse_button.middle) then
            syrup.input.set_cursor(syrup.input.cursor.normal)
            self.pan = nil
        end
    end,

    draw = function(self)
    end,

    load_instances = function(self)
        local insts = syrup.scenes.instances("SceneEditorB1")
        if not self.instances then
            io.write("Failed to load instances.\n")
        end

        self.instances = {}
        for i, inst in ipairs(insts) do
            self.instances[i] = syrup.objects.new("instance", inst.x, inst.y, inst.z, true, "")
            self.instances[i].child = syrup.objects.new(inst.type, 0, 0, 0, false, "")
        end
    end,

    draw_editor = function(self)
        syrup.graphics.draw_sprite("scene_editor_icon", self.x, self.y, 0)
    end,
}