require("engine.keyboard")
require("ui.menu")
require("ui.info")

return {
    depth = 0.0,
    start = function(this)
        this.file_button = object_new("ui/button", 0, 0)
        this.file_button.set_text(this.file_button, "FILE")
        this.file_button.on_click = function(button)
            list = object_new("ui/list", button.x, button.y + button.height)
            list.set_entries(list, {
                {
                    text = "IMPORT SPRITE",
                    on_click = function(_)
                        io.write("IMPORT\n")
                    end,
                },
                {
                    text = "EXIT",
                    on_click = function(this, index, entry)
                        entry.list = object_new("ui/list", entry.x + this.width, entry.y)
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
                { text = "DUPLICATE" },
                { text = "DELETE" },
            })
        end
    end,

    update = function(this)

    end,

    draw_gui = function(this)
        draw_rectangle(0, 0, dimensions.width, this.file_button.height, normal_color)
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