return {
    name = "pie",
    depth = 0.0,
    update = function(this)
        draw_rectangle(-30, -30, 30, 30, { r = 150, g = 150, b = 255, a = 75 })
    end
}