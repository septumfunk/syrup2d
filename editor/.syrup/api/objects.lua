require("syrup")

---[API] The engine's objects module, providing functionality for controlling and creating objects.
syrup.objects = {}

---[API] Create an object using the engine's objects system.
---@param type string
---@param x number
---@param y number
---@param z number
---@param enabled boolean
---@param creation_code string
---@return table|nil object
syrup.objects.new = function(type, x, y, z, enabled, creation_code)end

---[API] Delete an object using the engine's objects system.
---@param id integer
syrup.objects.delete = function(id)end

---[API] Get an object from the engine's internal object manager.
---@param id integer
---@return table|nil object
syrup.objects.get = function(id)end

---[API] Get an object of an arbitrary type from the engine's internal object manager.
---@param type string
---@return table objects
---@diagnostic disable-next-line: missing-return
syrup.objects.get_all= function(type)end