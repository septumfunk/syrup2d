require("syrup")

return {
    super_awesome_message = "Go fuck yourself!\n",

    update = function(this)
        if syrup.input.is_key_down(key.space) then
            io.write(this.super_awesome_message)
        end
    end,
}