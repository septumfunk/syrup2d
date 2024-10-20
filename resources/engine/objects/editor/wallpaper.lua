require("syrup")

return {
    depth = 999,
    draw_ui = function(_)
        syrup.graphics.draw_rectangle(0, 0, syrup.dimensions.width, syrup.dimensions.height, ui_color_background)
    end,
}