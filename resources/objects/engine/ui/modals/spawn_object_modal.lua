require("ui.info")

return {
    start = function(this)
        this.modal = object_new("engine/ui/modals/modal", 0, 0)
        this.modal.titlebar.title = "SPAWN OBJECT"
        this.modal.resize(this.modal, 200, 49)
        this.modal.cleanup = function(_)
            object_delete(this.id)
        end
        this.depth = this.modal.depth + 0.1
        this.modal.x = dimensions.width / 2 - this.modal.width / 2
        this.modal.y = dimensions.height / 2 - this.modal.height / 2

        this.object_type = object_new("engine/ui/textbox", 0, 0)
        this.object_type.max_length = 25;
        this.object_type.x = this.modal.x + this.modal.width / 2
        this.object_type.y = this.modal.y + ui_text_size.height + this.object_type.height / 2 + 4
        this.object_type.stick(this.object_type, this.modal)

        this.object_x = object_new("engine/ui/textbox", 0, 0)
        this.object_x.max_length = 4;
        this.object_x.start(this.object_x)
        this.object_x.x = this.object_type.x - this.object_x.width / 2 - ui_text_size.width - 2
        this.object_x.y = this.object_type.y + this.object_type.height / 2 + this.object_x.height / 2 + ui_text_size.height + 4
        this.object_x.stick(this.object_x, this.modal)
        this.object_x.text = tostring(0)

        this.object_y = object_new("engine/ui/textbox", 0, 0)
        this.object_y.max_length = 4;
        this.object_y.start(this.object_y)
        this.object_y.x = this.object_type.x + this.object_y.width / 2 + ui_text_size.width + 2
        this.object_y.y = this.object_x.y
        this.object_y.stick(this.object_y, this.modal)
        this.object_y.text = tostring(0)

        this.spawn_button = object_new("engine/ui/button", 0, 0)
        this.spawn_button.set_text(this.spawn_button, "SPAWN")
        this.spawn_button.x = this.modal.x + this.modal.width / 2 - this.spawn_button.width / 2
        this.spawn_button.y = this.modal.y + this.modal.height - this.spawn_button.height - 2
        this.spawn_button.stick(this.spawn_button, this.modal)

        this.spawn_button.on_click = function(_)
            if string.len(this.object_type.text) < 1 then
                return
            end

            local x = tonumber(this.object_x.text)
            if x == nil then
                this.object_x.text = ""
                return
            end
            x = math.floor(x)

            local y = tonumber(this.object_y.text)
            if y == nil then
                this.object_y.text = ""
                return
            end
            y = math.floor(y)

            if object_new(this.object_type.text, x, y) ~= nil then
                object_delete(this.modal.id)
            else
                this.object_type.text = ""
            end
        end
    end,

    update = function(this)
        this.depth = this.modal.depth + 0.1
    end,

    draw_gui = function(this)
        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.object_type.y - this.object_type.height / 2 - this.object_type.border - 2 - ui_text_size.height / 2, "OBJECT TYPE", ui_color_primary)

        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.object_x.y - this.object_x.height / 2 - this.object_x.border - 2 - ui_text_size.height / 2, "POSITION", ui_color_primary)
        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.object_x.y, ",", ui_color_primary)
    end,

    cleanup = function(this)
        object_delete(this.object_type.id)
        object_delete(this.object_x.id)
        object_delete(this.object_y.id)
        object_delete(this.spawn_button.id)
    end,
}