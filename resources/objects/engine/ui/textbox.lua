require("engine.keyboard")
require("engine.mouse")

return {
    selected = false,
    max_length = 10,
    width = ui_text_size.width * 10,
    padding = 2,
    border = 1,
    text = "",

    start = function(this)
        this.width = this.max_length * ui_text_size.width
        this.height = this.padding * 2 + ui_text_size.height
    end,

    update = function(this)
        if this.stuck_to ~= nil then
            this.x = this.stuck_to.x - this.stick_offset.x
            this.y = this.stuck_to.y - this.stick_offset.y
            this.depth = this.stuck_to.depth + 0.1
        end

        this.width = this.max_length * ui_text_size.width
        this.height = this.padding + ui_text_size.height

        local x = this.x - this.width / 2
        local y = this.y - this.height / 2
        local hovering = mouse.x > x and mouse.x < x + this.width and mouse.y > y and mouse.y < y + this.height
        if is_mouse_button_pressed(mouse_button.left)  then
            this.selected = hovering
        end

        if this.selected then
            this.text = this.text .. keyboard_string()
            this.text = string.sub(this.text, 0, this.max_length)
            if is_key_pressed(key.backspace) and string.len(this.text) > 0 then
                this. text = string.sub(this.text, 0, string.len(this.text) - 1)
            end
        end
    end,

    draw_gui = function(this)
        local x = this.x - this.width / 2
        local y = this.y - this.height / 2
        draw_rectangle(x - this.border, y - this.border, this.width + this.border * 2, this.height + this.border * 2, ui_color_secondary)
        if this.selected then
            draw_rectangle(x, y, this.width, this.height, ui_white)
            draw_ui_text_centered(this.x, this.y, this.text, ui_color_primary)
        else
            draw_rectangle(x, y, this.width, this.height, ui_color_primary)
            draw_ui_text_centered(this.x, this.y, this.text, ui_white)
        end
    end,

    stick = function(this, other)
        this.stuck_to = other
        this.stick_offset = {
            x = other.x - this.x,
            y = other.y - this.y,
        }
    end,
}