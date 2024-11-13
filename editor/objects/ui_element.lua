require("syrup")

return {
    hovering = false,
    width = 0,
    height = 0,

    update = function(self)
        self.hovering =
            syrup.mouse.x > self.x and
            syrup.mouse.x < self.x + self.width and
            syrup.mouse.y > self.y and
            syrup.mouse.y < self.y + self.height

        if self.hovering then
            syrup.input.set_cursor(syrup.input.cursor.pointer)
        end
    end,
}