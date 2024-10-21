require("syrup")

---[API] The resource management component of the engine's api.
syrup.resources = {}

---[API] Attempt to import a sprite into the resource manager and save it to a .spr.syr file.
---@param name string
---@param frame_count integer
---@param frame_delay integer
syrup.resources.import_sprite = function(name, frame_count, frame_delay)end

---[API] Access the game's configuration data from the lua api.
---@return table game_data
---@diagnostic disable-next-line: missing-return
syrup.resources.get_game_data = function()end

---[API] Update the game's configuration data from the lua api.
---@param title string
---@param width integer
---@param height integer
---@param window_scale integer
syrup.resources.set_game_data = function(title, width, height, window_scale)end