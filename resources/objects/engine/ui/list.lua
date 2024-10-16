require("ui.info")

return {
    entries = {},
    width = 0,
    height = 0,
    padding = 4,
    hovering = false,

    update = function(this)
        this.hovering = mouse.x > this.x and mouse.x < this.x + this.width and mouse.y > this.y and mouse.y < this.y + this.height
        for _, entry in pairs(this.entries) do
            if entry.list ~= nil then
                if mouse.x > entry.list.x and mouse.x < entry.list.x + entry.list.width and mouse.y > entry.list.y and mouse.y < entry.list.y + entry.list.height then
                    this.hovering = true
                    break
                end
            end
        end
        if is_mouse_button_pressed(mouse_button.left) and not this.hovering then
            object_delete(this.id)
        end
    end,

    draw_gui = function(this)
        x = this.x
        y = this.y
        for index, entry in ipairs(this.entries) do
            if mouse.x > x and mouse.x < x + this.width and mouse.y > y and mouse.y < y + ui_text_size.height + this.padding then
                if is_mouse_button_down(mouse_button.left) then
                    draw_rectangle(x, y, this.width, ui_text_size.height + this.padding, ui_color_pressed)
                else
                    draw_rectangle(x, y, this.width, ui_text_size.height + this.padding, ui_color_secondary)
                end
                draw_ui_text(x + this.padding / 2, y + this.padding / 2, entry.text, ui_white)

                if is_mouse_button_pressed(mouse_button.left) then
                    entry.x = x
                    entry.y = y
                    entry.on_click(this, index, entry)
                end
            else
                draw_rectangle(x, y, this.width, ui_text_size.height + this.padding, ui_color_primary)
                draw_ui_text(x + this.padding / 2, y + this.padding / 2, entry.text, ui_white)
            end
            y = y + ui_text_size.height + this.padding
        end
    end,

    set_entries = function(this, entries)
        this.entries = entries

        for _, entry in ipairs(this.entries) do
            local my_width = string.len(entry.text) * ui_text_size.width + this.padding
            if my_width > this.width then
                this.width = my_width
            end
        end

        this.height = 0
        for _ in pairs(this.entries) do
            this.height = this.height + ui_text_size.height + this.padding
        end
    end,

    cleanup = function(this)
        for _, entry in ipairs(this.entries) do
            if entry.list ~= nil then
                object_delete(entry.list.id)
            end
        end
    end,

    delete_all = function()
        for _, object in pairs(object_get_all("engine/ui/list")) do
            object_delete(object.id)
        end
    end,
}