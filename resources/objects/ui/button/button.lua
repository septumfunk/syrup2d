require("engine.mouse")
require("ui.info")
require("ui.text")

return {
    depth = 0.1,
    hovering = false,
    width = 0,
    height = 0,
    padding = 2,
    on_click = function(this)
        io.write("Button '" .. this.text .. "' clicked!")
    end,

    start = function(this)
        this.set_text(this, "default")
    end,

    update = function(this)
        this.hovering = mouse.x > this.x and mouse.x < this.x + this.width and mouse.y > this.y and mouse.y < this.y + this.height
        if is_mouse_button_pressed(mouse_button.left) and this.hovering then
            this.on_click(this)
        end

        if this.mounted ~= nil then
            if this.mounted.side == "top" then
                this.x = this.mounted.other.x
                this.y = this.mounted.other.y - this.mounted.other.height
            elseif this.mounted.side == "bottom" then
                this.x = this.mounted.other.x
                this.y = this.mounted.other.y + this.mounted.other.height
            elseif this.mounted.side == "left" then
                this.x = this.mounted.other.x - this.mounted.other.width
                this.y = this.mounted.other.y
            else
                this.x = this.mounted.other.x + this.mounted.other.width
                this.y = this.mounted.other.y
            end
        end

        if this.stuck_to ~= nil then
            this.x = this.stuck_to.x - this.stick_offset.x
            this.y = this.stuck_to.y - this.stick_offset.y
        end
    end,

    draw_gui = function(this)
        if this.hovering then
            draw_rectangle(this.x, this.y, this.width, this.height, ui_color_secondary)
            if is_mouse_button_down(mouse_button.left) then
                draw_rectangle(this.x, this.y, this.width, this.height, ui_color_pressed)
            else
                draw_rectangle(this.x, this.y, this.width, this.height, ui_color_secondary)
            end
        else
            draw_rectangle(this.x, this.y, this.width, this.height, ui_color_primary)
        end
        draw_ui_text(this.x + this.padding, this.y + this.padding, this.text, ui_white)

    end,

    set_text = function(this, text)
        this.text = text
        this.width = (ui_text_size.width * string.len(this.text)) + this.padding * 2
        this.height = ui_text_size.height + this.padding * 2
    end,

    mount = function(this, _other, _side)
        this.mounted = { other = _other, side = _side}
    end,

    stick = function(this, other)
        this.stuck_to = other
        this.stick_offset = {
            x = other.x - this.x,
            y = other.y - this.y,
        }
    end,
}