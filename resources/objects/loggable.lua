require("syrup")

return {
    log = function(self, text)
        io.write("Log: " .. text .. "\n")
    end,
}