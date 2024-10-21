require("syrup")

---[API] The resource management component of the engine's api.
syrup.resources = {}

---[API] Attempt to import a sprite into the resource manager and save it to a .spr.syr file.
---@param name string
---@param frame_count integer
---@param frame_delay integer
syrup.resources.import_sprite = function(name, frame_count, frame_delay)end