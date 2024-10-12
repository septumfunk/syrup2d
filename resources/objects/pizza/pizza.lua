return {
    name = "pie",
    depth = 0.0,
    update = function(this)

    end,
    draw = function(this)
        draw_rectangle(this.x, this.y, 20, 20, { r = 150, g = 150, b = 255, a = 75 })
    end,
}