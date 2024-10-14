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
                entry.list.hovering = mouse.x > entry.list.x and mouse.x < entry.list.x + entry.list.width and mouse.y > entry.list.y and mouse.y < entry.list.y + entry.list.height
                if entry.list.hovering then
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
            y = y + (index - 1) * (ui_text_size.height + this.padding)
            if mouse.x > x and mouse.x < x + this.width and mouse.y > y and mouse.y < y + ui_text_size.height + this.padding then
                draw_rectangle(x, y, this.width, ui_text_size.height + this.padding, hover_color)
                if is_mouse_button_pressed(mouse_button.left) then
                    entry.x = x
                    entry.y = y
                    entry.on_click(this, index, entry)
                end
            else
                draw_rectangle(x, y, this.width, ui_text_size.height + this.padding, normal_color)
            end
            draw_ui_text(x + this.padding / 2, y + this.padding / 2, entry.text)
        end
    end,

    set_entries = function(this, entries)
        this.entries = entries

        for _, entry in ipairs(this.entries) do
            if string.len(entry.text) > this.width then
                this.width = string.len(entry.text) * ui_text_size.width + this.padding
            end
        end

        this.height = 0
        for _ in pairs(this.entries) do
            this.height = this.height + ui_text_size.height + this.padding
        end
    end,
}