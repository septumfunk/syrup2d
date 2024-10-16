require("ui.info")
require("engine.mouse")

return {
    depth = 0.2,
    width = 20,
    height = 40,
    titlebar = {
        title = "Default",
        padding = 4,
    },
    border = 1,

    start = function(this)
        local others = object_get_all(this.type)
        local top
        for _, other in pairs(others) do
            if other.id ~= this.id and (top == nil or other.depth >= top) then
                top = other.depth
            end
        end
        if top ~= nil then
            this.depth = top + 0.2
        end

        -- Update titlebar
        if this.width < string.len(this.titlebar.title) * ui_text_size.width + 20 then
            this.width = string.len(this.titlebar.title) * ui_text_size.width + 20
        end
        this.titlebar.width = this.width
        this.titlebar.x = this.x
        this.titlebar.height = ui_text_size.height + this.titlebar.padding
        this.titlebar.y = this.y - this.titlebar.height
    end,

    update = function(this)
        -- Update titlebar
        if this.width < string.len(this.titlebar.title) * ui_text_size.width + 20 then
            this.width = string.len(this.titlebar.title) * ui_text_size.width + 20
        end
        this.titlebar.width = this.width
        this.titlebar.x = this.x
        this.titlebar.height = ui_text_size.height + this.titlebar.padding
        this.titlebar.y = this.y - this.titlebar.height - this.border

        -- Drag
        local hovering_titlebar = mouse.x > this.titlebar.x
        and mouse.x < this.titlebar.x + this.titlebar.width
        and mouse.y > this.titlebar.y
        and mouse.y < this.titlebar.y + this.titlebar.height

        local hovering_other = false
        local others = object_get_all(this.type)
        local top = this.depth
        for _, other in pairs(others) do
            if other.id ~= this.id then
                if other.depth >= top then
                    top = other.depth
                end
                if not hovering_other and other.depth > this.depth then
                    hovering_other = mouse.x > other.titlebar.x
                    and mouse.x < other.titlebar.x + other.titlebar.width
                    and mouse.y > other.titlebar.y
                    and mouse.y < other.titlebar.y + other.titlebar.height
                end
            end
        end

        if hovering_titlebar and is_mouse_button_pressed(mouse_button.left) and not hovering_other then
            if this.depth <= top then
                this.depth = top
                for _, other in pairs(others) do
                    if other.id ~= this.id then
                        other.depth = other.depth - 0.2
                    end
                end
            end
            this.offset = { x = mouse.x - this.x, y = mouse.y - this.y }
        end
        if this.offset ~= nil then
            if is_mouse_button_down(mouse_button.left) then
                this.x = math.min(dimensions.width - this.width - this.border, math.max(0 + this.border, mouse.x - this.offset.x))
                this.y = math.min(dimensions.height - this.height - this.border, math.max(0 + this.border + this.titlebar.height, mouse.y - this.offset.y))
            else
                this.offset = nil
            end
        end

        -- Close
        local hovering_close = mouse.x > this.titlebar.x + this.titlebar.width - this.titlebar.padding / 2 - ui_text_size.width
        and mouse.x < this.titlebar.x + this.titlebar.width - this.titlebar.padding / 2
        and mouse.y > this.titlebar.y + this.titlebar.padding / 2
        and mouse.y < this.titlebar.y + this.titlebar.padding / 2 + ui_text_size.height
        if hovering_close and is_mouse_button_pressed(mouse_button.left) and not hovering_other then
            object_delete(this.id)
        end
    end,

    draw_gui = function(this)
        -- Box
        draw_rectangle(this.x - this.border, this.y - this.border, this.width + this.border * 2, this.height + this.border * 2, ui_color_primary)
        draw_rectangle(this.x, this.y, this.width, this.height, ui_white)

        -- Title bar
        draw_rectangle(this.titlebar.x - this.border, this.titlebar.y - this.border, this.titlebar.width + this.border * 2, this.titlebar.height + this.border * 2, ui_color_primary)
        draw_ui_text(this.titlebar.x + this.titlebar.padding / 2, this.titlebar.y + this.titlebar.padding / 2, this.titlebar.title, ui_white)
        draw_rectangle(this.titlebar.x + this.titlebar.width - this.titlebar.padding / 2 - ui_text_size.width - 1,
        this.titlebar.y + this.titlebar.padding / 2 - 1, ui_text_size.width + 2, ui_text_size.height + 2, ui_red)
        draw_ui_text(this.titlebar.x + this.titlebar.width - this.titlebar.padding / 2 - ui_text_size.width, this.titlebar.y + this.titlebar.padding / 2, "x", ui_white)
    end,

    resize = function(this, width, height)
        if width < string.len(this.titlebar.title) * ui_text_size.width + 20 then
            width = string.len(this.titlebar.title) * ui_text_size.width + 20
        end
        this.width = width
        this.height = height
    end
}