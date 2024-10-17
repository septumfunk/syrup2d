return {
    start = function(this)
        io.write("Start\n")
    end,
    update = function(this)
        io.write("Update\n")
    end,
    draw = function(this)
        io.write("Draw\n")
    end,
    draw_gui = function(this)
        io.write("Draw GUI\n")
    end,
}