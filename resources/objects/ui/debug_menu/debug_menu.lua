require("engine.keyboard")
require("ui.info")

return {
    depth = -1,
    start = function(this)
        this.file_button = object_new("ui/button", 0, 0)
        this.file_button.set_text(this.file_button, "FILE")
        this.file_button.on_click = function(button)
            list = object_new("ui/list", button.x, button.y + button.height)
            list.set_entries(list, {
                {
                    text = "IMPORT",
                    on_click = function(this, _, entry)
                        entry.list = object_new("ui/list", entry.x + this.width, entry.y)
                        entry.list.set_entries(entry.list, {
                            {
                                text = "SPRITE",
                                on_click = function(_, _, entry)
                                    modal = object_new("ui/modal", 30, 30)
                                    modal.titlebar.title = "IMPORT SPRITE"
                                    modal.resize(modal, 0, 80)
                                    modal.sprite = "default"

                                    modal.import_button = object_new("ui/button", 0, 0)
                                    modal.import_button.set_text(modal.import_button, "IMPORT")
                                    modal.import_button.x = modal.x + modal.width / 2 - modal.import_button.width / 2
                                    modal.import_button.y = modal.y + 5
                                    modal.import_button.stick(modal.import_button, modal)

                                    modal.update_custom = function(modal)
                                        modal.width = sprite_width(modal.sprite) + 20
                                        modal.height = sprite_height(modal.sprite) + 40
                                    end
                                    modal.draw_custom = function(modal)
                                        draw_sprite(modal.sprite, modal.x + modal.width / 2 - sprite_width(modal.sprite) / 2, modal.y + modal.height - 10 - sprite_height(modal.sprite), 0)
                                    end
                                    modal.cleanup = function(modal)
                                        object_delete(modal.import_button.id)
                                    end

                                    entry.list.delete_all()
                                end,
                            },
                        })
                    end,
                },
                {
                    text = "EXIT",
                    on_click = function(_, _, _)
                        game_end()
                    end
                },
            })
        end

        this.edit_button = object_new("ui/button", 0, 0)
        this.edit_button.set_text(this.edit_button, "EDIT")
        this.edit_button.mount(this.edit_button, this.file_button, "right")
        this.edit_button.on_click = function(button)
            local list = object_new("ui/list", button.x, button.y + button.height)
            list.set_entries(list, {
                {
                    text = "EDIT GAME DATA",
                    on_click = function(this, _, entry)
                        modal = object_new("ui/modals/game_data_modal", 30, 30)
                        this.delete_all()
                    end
                },
            })
        end
    end,

    update = function(this)

    end,

    draw_gui = function(this)
        draw_sprite("wallpaper", 0, 0, 0)
        draw_rectangle(0, 0, dimensions.width, this.file_button.height, ui_color_primary)
        this.file_menu.draw(this)
    end,

    file_menu = {
        open = false,
        draw = function(this)
            if this.file_menu.open then
                draw_menu(
                    0,
                    button_height,
                    {
                        {
                            text = "IMPORT SPRITE",
                            on_click = function()
                                io.write("Dummy\n")
                            end,
                        },
                        {
                            text = "EXIT",
                            on_click = function()
                                game_end()
                            end,
                        }
                    }
                )

                if is_mouse_button_pressed(mouse_button.left) then
                    this.file_menu.open = false
                    return
                end
            end
        end
    },
}