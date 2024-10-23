require("syrup")

return {
    min_width = 80,
    min_height = 80,

    padding = 1,
    border = 1,
    extend = { dir = nil, offset = { x = 0, y = 0 } },
    hovering = false,
    resizable = true,

    start = function(self)
        self:set_title("Window")

        self.close_button = syrup.objects.new("editor/button", -999, -999)
        self.close_button.padding = 0
        self.close_button:set_text("X")
        self.close_button.on_click = function(_)
            syrup.objects.delete(self.id)
        end

        local others = syrup.objects.get_all("editor/window")
        local highest = self
        for _, window in ipairs(others) do
            if window.id == self.id then
                goto continue
            end
            if window.depth < highest.depth then
                highest = window
            end
            ::continue::
        end
        self.depth = highest.depth - 0.2
    end,

    update = function(self)
        local hovering_titlebar =
        syrup.mouse.x > self.x - self.padding and
        syrup.mouse.x < self.x + self.width + self.padding * 2 and
        syrup.mouse.y > self.y - syrup.ui.font_size.height - self.padding * 3 and
        syrup.mouse.y < self.y - self.padding

        if self.offset ~= nil then
            self.x = syrup.mouse.x - self.offset.x
            self.y = syrup.mouse.y - self.offset.y
            if not syrup.input.is_mouse_button_down(syrup.input.mouse_button.left) then
                self.offset = nil
            end
        elseif hovering_titlebar and syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
            self.offset = {
                x = syrup.mouse.x - self.x,
                y = syrup.mouse.y - self.y,
            }
        end

        local hovering_window_leeway = 6
        local left = self.x - self.padding - self.border
        local right = self.x + self.width + self.padding + self.border
        local top = self.y - syrup.ui.font_size.height - self.padding * 4 - self.border
        local bottom = self.y + self.height + self.padding + self.border
        local hovering_window =
        syrup.mouse.x > left - hovering_window_leeway and
        syrup.mouse.x < right + hovering_window_leeway and
        syrup.mouse.y > top - hovering_window_leeway and
        syrup.mouse.y < bottom + hovering_window_leeway
        local cursor_set = false

        if (hovering_window or self.extend.dir) and self.resizable then
            local new_x = syrup.mouse.x - self.extend.offset.x
            local new_y = syrup.mouse.y - self.extend.offset.y

            if not self:is_overlapped() and self.extend.dir == "up left" then
                cursor_set = true
                if (self.width <= self.min_width and new_x < self.x) or self.width > self.min_width then
                    self.width = self.width - (new_x - self.x)
                    self.x = new_x
                end
                if (self.height <= self.min_height and new_y < self.y) or self.height > self.min_height then
                    self.height = self.height - (new_y - self.y)
                    self.y = new_y
                end
            elseif not self:is_overlapped() and self.extend.dir == "down left" then
                cursor_set = true
                if (self.width <= self.min_width and new_x < self.x) or self.width > self.min_width then
                    self.width = self.width - (new_x - self.x)
                    self.x = new_x
                end
                if (self.height <= self.min_height and new_y > self.y) or self.height > self.min_height then
                    self.height = syrup.mouse.y - self.y
                end
            elseif not self:is_overlapped() and self.extend.dir == "up right" then
                cursor_set = true
                if (self.width <= self.min_width and new_x > self.x) or self.width > self.min_width then
                    self.width = syrup.mouse.x - self.x
                end
                if (self.height <= self.min_height and new_y < self.y) or self.height > self.min_height then
                    self.height = self.height - (new_y - self.y)
                    self.y = new_y
                end
            elseif not self:is_overlapped() and self.extend.dir == "down right" then
                cursor_set = true
                if (self.width <= self.min_width and new_x > self.y) or self.width > self.min_width then
                    self.width = syrup.mouse.x - self.x
                end
                if (self.height <= self.min_height and new_y > self.y) or self.height > self.min_height then
                    self.height = syrup.mouse.y - self.y
                end
            elseif not self:is_overlapped() and self.extend.dir == "left" then
                cursor_set = true
                if (self.width <= self.min_width and new_x < self.x) or (self.width > self.min_width and new_x < self.x + self.width / 2) then
                    self.width = self.width - (new_x - self.x)
                    self.x = new_x
                end
            elseif not self:is_overlapped() and self.extend.dir == "right" then
                cursor_set = true
                if (self.width <= self.min_width and new_x > self.x) or self.width > self.min_width then
                    self.width = syrup.mouse.x - self.x
                end
            elseif not self:is_overlapped() and self.extend.dir == "bottom" then
                cursor_set = true
                if (self.height <= self.min_height and new_y > self.y) or self.height > self.min_height then
                    self.height = syrup.mouse.y - self.y
                end
            elseif not self:is_overlapped() and self.extend.dir == "top" then
                cursor_set = true
                if (self.height <= self.min_height and new_y < self.y) or (self.height > self.min_height and new_y < self.y + self.height / 2) then
                    self.height = self.height - (new_y - self.y)
                    self.y = new_y
                end
            elseif hovering_window and syrup.mouse.x < left + self.border
            and syrup.mouse.y < top + self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.nwseresize)
                if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "up left",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            elseif hovering_window and syrup.mouse.x < left + self.border
            and syrup.mouse.y > bottom - self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.neswresize)
                    if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "down left",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            elseif hovering_window and syrup.mouse.x > right - self.border
            and syrup.mouse.y < top + self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.neswresize)
                if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "up right",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            elseif hovering_window and syrup.mouse.x > right - self.border
            and syrup.mouse.y > bottom - self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.nwseresize)
                if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "down right",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            elseif hovering_window and syrup.mouse.x < left + self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.hresize)
                if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "left",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            elseif hovering_window and syrup.mouse.x > right - self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.hresize)
                if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "right",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            elseif hovering_window and syrup.mouse.y > bottom - self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.vresize)
                if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "bottom",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            elseif hovering_window and syrup.mouse.y < top + self.border then
                cursor_set = true
                syrup.input.set_cursor(syrup.input.cursor.vresize)
                if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) and self:to_top() then
                    self.extend = {
                        dir = "top",
                        offset = { x = syrup.mouse.x - self.x, y = syrup.mouse.y - self.y }
                    }
                end
            end
        end

        if not cursor_set and self.cursor_set then
            syrup.input.set_cursor(syrup.input.cursor.normal)
        end
        self.cursor_set = cursor_set

        if not syrup.input.is_mouse_button_down(syrup.input.mouse_button.left) then
            self.extend.dir = nil
        end

        self.width = math.max(self.width, self.min_width)
        self.height = math.max(self.height, self.min_height)

        self.close_button.x = self.x + self.width + self.padding * 2 - self.close_button.width - self.border
        self.close_button.y = self.y - syrup.ui.font_size.height - self.padding * 3 - self.border
        self.close_button:stick(self)
    end,

    draw_ui = function(self)
        local titlebar_height = syrup.ui.font_size.height + self.padding * 2
        local border_x = self.x - self.padding * 2 - self.border
        local border_y = self.y - self.padding * 3 - self.border - titlebar_height
        local border_width = self.width + self.padding * 4 + self.border * 2
        local border_height = self.height + self.padding * 5 + self.border * 2 + titlebar_height
        local padding_x = self.x - self.padding * 2
        local padding_y = self.y - self.padding * 3 - titlebar_height
        local padding_width = self.width + self.padding * 4
        local padding_height = self.height + self.padding * 5 + titlebar_height

        syrup.graphics.draw_rectangle(border_x, border_y, border_width, border_height, syrup.ui.white) -- White of border
        syrup.graphics.draw_rectangle(border_x + 1, border_y + 1, border_width - self.border, border_height - self.border, syrup.ui.black) -- Black of border
        syrup.graphics.draw_rectangle(padding_x, padding_y, padding_width, padding_height, syrup.ui.primary)

        syrup.graphics.draw_rectangle(self.x - self.padding, self.y - titlebar_height - self.padding * 2, padding_width - self.padding * 2, titlebar_height, syrup.ui.accent)
        syrup.graphics.draw_text("ui_font", self.x + self.padding, self.y - titlebar_height, self._title, syrup.ui.white)

        syrup.graphics.draw_rectangle(self.x - self.padding, self.y - self.padding, self.width + self.padding * 2, self.height + self.padding * 2, syrup.ui.black)
        syrup.graphics.draw_rectangle(self.x, self.y, self.width + self.padding, self.height + self.padding, syrup.ui.white)
        syrup.graphics.draw_rectangle(self.x, self.y, self.width, self.height, syrup.ui.primary)
    end,

    clean_up = function(self)
        syrup.objects.delete(self.close_button.id)
    end,

    set_title = function(self, title)
        self._title = title
        self.min_width = math.max(self.min_width, string.len(title) * syrup.ui.font_size.width + 10)

        self.width = self.min_width
        self.height = self.min_height
    end,

    is_overlapped = function(self)
        for _, window in ipairs(syrup.objects.get_all("editor/window")) do
            if window.id ~= self.id and window.depth < self.depth then
                if syrup.mouse.x > window.x - window.padding - window.border and
                syrup.mouse.x < window.x + window.width + window.padding + window.border and
                syrup.mouse.y > window.y - syrup.ui.font_size.height - window.padding * 4 - window.border and
                syrup.mouse.y < window.y + window.height + window.padding + window.border then
                    return true -- Overlap
                end
            end
        end
    end,

    to_top = function(self)
        if self:is_overlapped() then
            return false
        end
        local others = syrup.objects.get_all("editor/window")
        local highest = self
        for _, window in ipairs(others) do
            if window.id == self.id then
                goto continue
            end
            if window.depth < highest.depth then
                highest = window
            end
            ::continue::
        end
        if highest ~= self then
            self.depth = highest.depth
            for _, window in ipairs(syrup.objects.get_all("editor/window")) do
                if window.id ~= self.id then
                    window.depth = window.depth + 0.2
                end
            end
        end
        return true
    end,
}