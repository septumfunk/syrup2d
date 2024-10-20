require("syrup")

---[API] Syrup's graphics module. Exposes renderer functions to lua.
syrup.graphics = {}

---[API] Draw a rectangle using Syrup's renderer.
---@param x number
---@param y number
---@param width number
---@param height number
---@param color table
syrup.graphics.draw_rectangle = function(x, y, width, height, color)end

---[API] Draw a sprite using Syrup's renderer.
---@param name string
---@param x number
---@param y number
---@param frame integer
syrup.graphics.draw_sprite = function(name, x, y, frame)end

---[API] Draw a sprite using Syrup's renderer with a color tint applied.
---@param name string
---@param x number
---@param y number
---@param frame integer
---@param color table
syrup.graphics.draw_sprite_tint = function(name, x, y, frame, color)end

---[API] Draw text made using a sprite.
---@param name string
---@param x number
---@param y number
---@param text string
---@param color table
syrup.graphics.draw_text = function(name, x, y, text, color)end