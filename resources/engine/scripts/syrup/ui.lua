syrup.ui = {
    font_size = { width = 8, height = 11 },
    white = { r = 253, g = 255, b = 255, a = 255 },
    black = { r = 0, g = 0, b = 0, a = 255 },
    primary = { r = 240, g = 240, b = 240, a = 255 },
    secondary = { r = 215, g = 215, b = 215, a = 255 },
    darkedge = { r = 147, g = 147, b = 147, a = 255 },
    darkborder = { r = 107, g = 107, b = 107, a = 255 },
    lightedge = { r = 227, g = 227, b = 227, a = 255 },
    lightborder = { r = 253, g = 255, b = 255, a = 255 },
    accent = { r = 2, g = 0, b = 128, a = 255 },
    background = { r = 59, g = 110, b = 165, a = 255 },

    draw_classic_box = function(x, y, width, height, depressed)
        if not depressed then
            syrup.graphics.draw_rectangle(x, y, width, height, syrup.ui.darkborder)
            syrup.graphics.draw_rectangle(x, y, width - 1, height - 1, syrup.ui.lightborder)

            syrup.graphics.draw_rectangle(x + 1, y + 1, width - 2, height - 2, syrup.ui.darkedge)
            syrup.graphics.draw_rectangle(x + 1, y + 1, width - 3, height - 3, syrup.ui.lightedge)

            syrup.graphics.draw_rectangle(x + 2, y + 2, width - 4, height - 4, syrup.ui.primary)
        else
            syrup.graphics.draw_rectangle(x, y, width, height, syrup.ui.lightborder)
            syrup.graphics.draw_rectangle(x, y, width - 1, height - 1, syrup.ui.darkborder)

            syrup.graphics.draw_rectangle(x + 1, y + 1, width - 2, height - 2, syrup.ui.lightedge)
            syrup.graphics.draw_rectangle(x + 1, y + 1, width - 3, height - 3, syrup.ui.darkedge)

            syrup.graphics.draw_rectangle(x + 2, y + 2, width - 4, height - 4, syrup.ui.primary)
        end
    end,
}