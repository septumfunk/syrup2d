require("syrup")

return {
    update = function(self)
        self:log()
    end,

    log = function(_)
        io.write("yeah\n")
    end,
}