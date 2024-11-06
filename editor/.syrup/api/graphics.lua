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

---[API] Draw a sprite using Syrup's renderer with extra options.
---@param name string
---@param x number
---@param y number
---@param x_scale number
---@param y_scale number
---@param rotation number
---@param frame integer
---@param tint table
syrup.graphics.draw_sprite_pro = function(name, x, y, x_scale, y_scale, rotation, frame, tint)end

---[API] Get a sprite's dimensions via the api.
---@param name string
---@return table dimensions
---@diagnostic disable-next-line: missing-return
syrup.graphics.sprite_dimensions = function(name)end

---[API] Draw text made using a sprite.
---@param name string
---@param x number
---@param y number
---@param text string
---@param color table
syrup.graphics.draw_text = function(name, x, y, text, color)end

---[API] Get camera's current position from the top left of the viewport.
---@return table position
---@diagnostic disable-next-line: missing-return
syrup.graphics.get_camera_position = function()end

---[API] Set camera's position from the top left of the viewport.
---@param x number
---@param y number
syrup.graphics.set_camera_position = function(x, y)end

---[API] Get camera's current position from the center of the viewport.
---@return table position
---@diagnostic disable-next-line: missing-return
syrup.graphics.get_camera_center = function()end

---[API] Set camera's position from the center of the viewport.
---@param x number
---@param y number
syrup.graphics.set_camera_center = function(x, y)end