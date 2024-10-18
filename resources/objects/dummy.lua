require("syrup")

return {
    extends = "loggable",

    update = function(self)
        self:log("Test")
    end,
}