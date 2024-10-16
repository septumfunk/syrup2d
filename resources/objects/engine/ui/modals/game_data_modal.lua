require("ui.info")

return {
    start = function(this)
        this.modal = object_new("engine/ui/modals/modal", 0, 0)
        this.modal.titlebar.title = "EDIT GAME DATA"
        this.modal.resize(this.modal, 200, 67)
        this.modal.cleanup = function(_)
            object_delete(this.id)
        end
        this.depth = this.modal.depth + 0.1
        this.modal.x = dimensions.width / 2 - this.modal.width / 2
        this.modal.y = dimensions.height / 2 - this.modal.height / 2

        this.save_button = object_new("engine/ui/button", 0, 0)
        this.save_button.set_text(this.save_button, "SAVE AND EXIT")
        this.save_button.x = this.modal.x + this.modal.width / 2 - this.save_button.width / 2
        this.save_button.y = this.modal.y + this.modal.height - this.save_button.height - 2
        this.save_button.stick(this.save_button, this.modal)

        local game_data = get_game_data();

        this.title_box = object_new("engine/ui/textbox", 0, 0)
        this.title_box.max_length = 25;
        this.title_box.x = this.modal.x + this.modal.width / 2
        this.title_box.y = this.modal.y + ui_text_size.height + this.title_box.height / 2 + 4
        this.title_box.stick(this.title_box, this.modal)
        this.title_box.text = game_data.title

        this.x_box = object_new("engine/ui/textbox", 0, 0)
        this.x_box.max_length = 4;
        this.x_box.start(this.x_box)
        this.x_box.x = this.title_box.x - this.x_box.width / 2 - ui_text_size.width - 2
        this.x_box.y = this.title_box.y + this.title_box.height / 2 + this.x_box.height / 2 + ui_text_size.height + 4
        this.x_box.stick(this.x_box, this.modal)
        this.x_box.text = tostring(math.floor(game_data.game_width))

        this.y_box = object_new("engine/ui/textbox", 0, 0)
        this.y_box.max_length = 4;
        this.y_box.start(this.y_box)
        this.y_box.x = this.title_box.x + this.y_box.width / 2 + ui_text_size.width + 2
        this.y_box.y = this.x_box.y
        this.y_box.stick(this.y_box, this.modal)
        this.y_box.text = tostring(math.floor(game_data.game_height))

        this.scale_box = object_new("engine/ui/textbox", 0, 0)
        this.scale_box.max_length = 1;
        this.scale_box.start(this.y_box)
        this.scale_box.x = this.title_box.x
        this.scale_box.y = this.x_box.y + this.title_box.height / 2 + this.scale_box.height / 2 + ui_text_size.height + 4
        this.scale_box.stick(this.scale_box, this.modal)
        this.scale_box.text = tostring(math.floor(game_data.window_scale))

        this.save_button.on_click = function(_)
            if string.len(this.title_box.text) < 1 then
                return
            end

            local width = tonumber(this.x_box.text)
            if width == nil then
                this.x_box.text = ""
                return
            end
            width = math.floor(width)

            local height = tonumber(this.y_box.text)
            if height == nil then
                this.y_box.text = ""
                return
            end
            height = math.floor(height)

            local window_scale = tonumber(this.scale_box.text)
            if window_scale == nil then
                this.scale_box.text = ""
                return
            end
            window_scale = math.floor(window_scale)

            set_game_data(this.title_box.text, width, height, window_scale)
        end
    end,

    update = function(this)
        this.depth = this.modal.depth + 0.1
    end,

    draw_gui = function(this)
        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.title_box.y - this.title_box.height / 2 - this.title_box.border - 2 - ui_text_size.height / 2, "TITLE", ui_color_primary)

        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.x_box.y - this.x_box.height / 2 - this.x_box.border - 2 - ui_text_size.height / 2, "DIMENSIONS", ui_color_primary)
        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.x_box.y, "X", ui_color_primary)

        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.scale_box.y - this.scale_box.height / 2 - this.scale_box.border - 2 - ui_text_size.height / 2, "WINDOW SCALE", ui_color_primary)
    end,

    cleanup = function(this)
        object_delete(this.save_button.id)
        object_delete(this.title_box.id)
        object_delete(this.x_box.id)
        object_delete(this.y_box.id)
        object_delete(this.scale_box.id)
    end,
}