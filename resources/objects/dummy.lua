require("syrup")

return {
    update = function(self)
        self:log()
    end,

    log = function(self)
        io.write("yeah\n")
    end,
}