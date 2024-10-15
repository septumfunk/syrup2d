require("ui.info")

return {
    start = function(this)
        this.modal = object_new("ui/modals/modal", 0, 0)
        this.modal.titlebar.title = "IMPORT SPRITE"
        this.modal.resize(this.modal, 120, 67)
        this.modal.cleanup = function(_)
            object_delete(this.id)
        end
        this.depth = this.modal.depth + 0.1
        this.modal.x = dimensions.width / 2 - this.modal.width / 2
        this.modal.y = dimensions.height / 2 - this.modal.height / 2

        this.import = object_new("ui/button", 0, 0)
        this.import.set_text(this.import, "IMPORT")
        this.import.x = this.modal.x + this.modal.width / 2 - this.import.width / 2
        this.import.y = this.modal.y + this.modal.height - this.import.height - 2
        this.import.stick(this.import, this.modal)

        local game_data = get_game_data();

        this.sprite_name = object_new("ui/textbox", 0, 0)
        this.sprite_name.max_length = 25;
        this.sprite_name.x = this.modal.x + this.modal.width / 2
        this.sprite_name.y = this.modal.y + ui_text_size.height + this.sprite_name.height / 2 + 4
        this.sprite_name.stick(this.sprite_name, this.modal)
        this.sprite_name.text = game_data.title

        this.frame_count = object_new("ui/textbox", 0, 0)
        this.frame_count.max_length = 4;
        this.frame_count.start(this.frame_count)
        this.frame_count.x = this.sprite_name.x - this.frame_count.width / 2 - 12
        this.frame_count.y = this.sprite_name.y + this.sprite_name.height / 2 + this.frame_count.height / 2 + ui_text_size.height + 4
        this.frame_count.stick(this.frame_count, this.modal)
        this.frame_count.text = tostring(math.floor(game_data.game_width))

        this.frame_delay = object_new("ui/textbox", 0, 0)
        this.frame_delay.max_length = 4;
        this.frame_delay.start(this.frame_delay)
        this.frame_delay.x = this.sprite_name.x + this.frame_delay.width / 2 + 12
        this.frame_delay.y = this.frame_count.y
        this.frame_delay.stick(this.frame_delay, this.modal)
        this.frame_delay.text = tostring(math.floor(game_data.game_height))

        this.import.on_click = function(_)

        end
    end,

    update = function(this)
        this.depth = this.modal.depth + 0.1
    end,

    draw_gui = function(this)
        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.sprite_name.y - this.sprite_name.height / 2 - this.sprite_name.border - 2 - ui_text_size.height / 2, "TITLE", ui_color_primary)

        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.frame_count.y - this.frame_count.height / 2 - this.frame_count.border - 2 - ui_text_size.height / 2, "DIMENSIONS", ui_color_primary)
        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.frame_count.y, "X", ui_color_primary)

        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.scale_box.y - this.scale_box.height / 2 - this.scale_box.border - 2 - ui_text_size.height / 2, "WINDOW SCALE", ui_color_primary)
    end,

    cleanup = function(this)
        object_delete(this.import.id)
        object_delete(this.sprite_name.id)
        object_delete(this.frame_count.id)
        object_delete(this.frame_delay.id)
        object_delete(this.scale_box.id)
    end,
}