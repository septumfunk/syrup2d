require("syrup")

return {
    _stuck_to = nil,
    _stuck_offset = { x = 0, y = 0 },
    hovering = false,
    centered = false,

    update = function(self)
        if self._stuck_to ~= nil then
            self.x = self._stuck_to.x - self._stuck_offset.x
            self.y = self._stuck_to.y - self._stuck_offset.y
            self.depth = self._stuck_to.depth - 0.1
        end

        local x = self.x
        local y = self.y
        if self.centered then
            x = x - self.width / 2
            y = y - self.height / 2
        end

        local hovering = syrup.mouse.x > x
        and syrup.mouse.x < x + self.width
        and syrup.mouse.y > y
        and syrup.mouse.y < y + self.height
        if hovering and not self.hovering then
            syrup.input.set_cursor(syrup.input.cursor.pointer)
        end
        if self.hovering and not hovering then
            syrup.input.set_cursor(syrup.input.cursor.normal)
        end
        self.hovering = hovering
    end,

    stick = function(self, to)
        self._stuck_to = to
        self._stuck_offset = {
            x = to.x - self.x,
            y = to.y - self.y,
        }
    end
}