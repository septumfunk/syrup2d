require("ui.info")

return {
    start = function(this)
        this.modal = object_new("engine/ui/modals/modal", 0, 0)
        this.modal.titlebar.title = "OPEN SPRITE"
        this.modal.resize(this.modal, 120, 31)
        this.modal.cleanup = function(_)
            object_delete(this.id)
        end
        this.depth = this.modal.depth + 0.1
        this.modal.x = dimensions.width / 2 - this.modal.width / 2
        this.modal.y = dimensions.height / 2 - this.modal.height / 2

        this.sprite_name = object_new("engine/ui/textbox", 0, 0)
        this.sprite_name.max_length = 15;
        this.sprite_name.x = this.modal.x + this.modal.width / 2
        this.sprite_name.y = this.modal.y + ui_text_size.height + this.sprite_name.height / 2 + 4
        this.sprite_name.stick(this.sprite_name, this.modal)

        this.open_button = object_new("engine/ui/button", 0, 0)
        this.open_button.set_text(this.open_button, "OPEN")
        this.open_button.x = this.modal.x + this.modal.width / 2 - this.open_button.width / 2
        this.open_button.y = this.modal.y + this.modal.height - this.open_button.height - 2
        this.open_button.stick(this.open_button, this.modal)

        this.open_button.on_click = function(_)
            if string.len(this.sprite_name.text) < 1 then
                return
            end

            local spr_view = object_new("engine/ui/modals/sprite_view_modal", 0, 0)
            spr_view.set_sprite(spr_view, this.sprite_name.text)
            object_delete(this.modal.id)
        end
    end,

    update = function(this)
        this.depth = this.modal.depth + 0.1
    end,

    draw_gui = function(this)
        draw_ui_text_centered(this.modal.x + this.modal.width / 2, this.sprite_name.y - this.sprite_name.height / 2 - this.sprite_name.border - 2 - ui_text_size.height / 2, "SPRITE NAME", ui_color_primary)
    end,

    cleanup = function(this)
        object_delete(this.sprite_name.id)
        object_delete(this.open_button.id)
    end,
}