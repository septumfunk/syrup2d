require("syrup")

---[API] The module of syrup's lua api responsible for keyboard and mouse inputs.
syrup.input = {}

---[API] Check if a key on the keyboard was pressed on this frame.
---@param key integer A keycode to check.
---@return boolean is_down Whether the key was pressed this frame or not.
---@diagnostic disable-next-line: missing-return
syrup.input.is_key_pressed = function(key)end

---[API] Check if a key on the keyboard is held down on this frame.
---@param key integer A keycode to check.
---@return boolean is_down Whether the key is held down this frame or not.
---@diagnostic disable-next-line: missing-return
syrup.input.is_key_down = function(key)end

---[API] Check if a mouse button has been pressed this frame.
---@param button integer The physical button on the mouse to check.
---@return boolean is_down Whether the button was pressed on this frame or not.
---@diagnostic disable-next-line: missing-return
syrup.input.is_mouse_button_pressed = function(button)end

---[API] Check if a mouse button is held down on this frame.
---@param button integer The physical button on the mouse to check.
---@return boolean is_down Whether the button is held down on this frame or not.
---@diagnostic disable-next-line: missing-return
syrup.input.is_mouse_button_down = function(button)end

---[API] Get the most recent keys pressed as a string.
---@return string keyboard_string The last keys pressed on the keyboard as a string.
---@diagnostic disable-next-line: missing-return
syrup.input.keyboard_string = function()end