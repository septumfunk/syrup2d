require("engine.keyboard")
require("ui.info")

return {
    depth = -1,
    start = function(this)
        this.file_button = object_new("engine/ui/button", 0, 0)
        this.file_button.set_text(this.file_button, "FILE")
        this.file_button.on_click = function(button)
            list = object_new("engine/ui/list", button.x, button.y + button.height)
            list.set_entries(list, {
                {
                    text = "OPEN",
                    on_click = function(this, _, entry)
                        entry.list = object_new("engine/ui/list", entry.x + this.width, entry.y)
                        entry.list.set_entries(entry.list, {
                            {
                                text = "SPRITE",
                                on_click = function(this, _, _)
                                    modal = object_new("engine/ui/modals/sprite_open_modal", 0, 0)
                                    this.delete_all()
                                end,
                            },
                        })
                    end,
                },
                {
                    text = "IMPORT",
                    on_click = function(this, _, entry)
                        entry.list = object_new("engine/ui/list", entry.x + this.width, entry.y)
                        entry.list.set_entries(entry.list, {
                            {
                                text = "SPRITE",
                                on_click = function(this, _, _)
                                    modal = object_new("engine/ui/modals/sprite_import_modal", 0, 0)
                                    this.delete_all()
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

        this.game_button = object_new("engine/ui/button", 0, 0)
        this.game_button.set_text(this.game_button, "GAME")
        this.game_button.mount(this.game_button, this.file_button, "right")
        this.game_button.on_click = function(button)
            local list = object_new("engine/ui/list", button.x, button.y + button.height)
            list.set_entries(list, {
                {
                    text = "EDIT GAME DATA",
                    on_click = function(this, _, _)
                        modal = object_new("engine/ui/modals/game_data_modal", 0, 0)
                        this.delete_all()
                    end
                },
                {
                    text = "SPAWN OBJECT",
                    on_click = function(this, _, _)
                        modal = object_new("engine/ui/modals/spawn_object_modal", 0, 0)
                        this.delete_all()
                    end
                },
            })
        end
    end,

    draw_gui = function(this)
        draw_sprite("wallpaper", 0, 0, 0)
        draw_rectangle(0, 0, dimensions.width, this.file_button.height, ui_color_primary)
    end,

    cleanup = function(this)
        object_delete(this.file_button.id)
        object_delete(this.game_button.id)
    end,
}