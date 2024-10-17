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