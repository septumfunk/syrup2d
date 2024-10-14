require("engine.mouse")
require("ui.info")
require("ui.text")

return {
    depth = 0.1,
    hovering = false,
    width = 0,
    height = 0,
    padding = 4,
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
    end,

    draw_gui = function(this)
        if this.hovering then
            draw_rectangle(this.x, this.y, this.width, this.height, hover_color)
        else
            draw_rectangle(this.x, this.y, this.width, this.height, normal_color)
        end
        draw_ui_text(this.x + this.padding / 2, this.y + this.padding / 2, this.text)
    end,

    set_text = function(this, text)
        this.text = text
        this.width = (ui_text_size.width * string.len(this.text)) + this.padding
        this.height = ui_text_size.height + this.padding
    end,

    mount = function(this, _other, _side)
        this.mounted = { other = _other, side = _side}
    end,
}