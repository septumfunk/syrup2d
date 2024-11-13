require("syrup")

---[API] The resource management component of the engine's api.
syrup.resources = {}

---[API] Attempt to import a sprite into the resource manager and save it to a .spr.syr file.
---@param name string
---@param frame_count integer
---@param frame_delay integer
syrup.resources.import_sprite = function(name, frame_count, frame_delay)end

---[API] Access the game's configuration data from the lua api.
---@return table metadata
---@diagnostic disable-next-line: missing-return
syrup.resources.get_metadata = function()end

---[API] Update the game's configuration data from the lua api.
---@param title string
---@param width integer
---@param height integer
---@param window_scale integer
syrup.resources.set_metadata = function(title, width, height, window_scale)end