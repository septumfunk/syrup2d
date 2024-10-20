require("syrup")

---[API] The module of syrup's lua api responsible for keyboard and mouse inputs.
syrup.input = {}

---[API] Check if a key on the keyboard was pressed on this frame.
---@param key integer
---@return boolean is_pressed
---@diagnostic disable-next-line: missing-return
syrup.input.is_key_pressed = function(key)end

---[API] Check if a key on the keyboard is held down on this frame.
---@param key integer
---@return boolean is_down
---@diagnostic disable-next-line: missing-return
syrup.input.is_key_down = function(key)end

---[API] Check if a mouse button has been pressed this frame.
---@param button integer
---@return boolean is_pressed
---@diagnostic disable-next-line: missing-return
syrup.input.is_mouse_button_pressed = function(button)end

---[API] Check if a mouse button is held down on this frame.
---@param button integer
---@return boolean is_down
---@diagnostic disable-next-line: missing-return
syrup.input.is_mouse_button_down = function(button)end

---[API] Get the most recent keys pressed as a string.
---@return string keyboard_string
---@diagnostic disable-next-line: missing-return
syrup.input.keyboard_string = function()end

---[API] Set the mouse cursor to the desired cursor icon.
--- You should use the `syrup.input.cursor` table for this.
---@param cursor integer
syrup.input.set_cursor = function(cursor)end