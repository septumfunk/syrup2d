require("syrup")

return {
    child = nil,
    selected = false,

    draw = function(self)
        if not self.child then
            return
        end

        if self.child.draw_editor then
            self.child:draw_editor()
        else
            syrup.graphics.draw_rectangle(self.x - 10, self.y - 10, 20, 20, syrup.ui.accent)
        end
    end,
}