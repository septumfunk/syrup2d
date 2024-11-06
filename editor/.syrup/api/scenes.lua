require("syrup")

--- [API] The scenes module of the syrup api, responsbile for handling scenes and their properties. All functions that modify the scene also save it.
syrup.scenes = {}

--- [API] Returns a table containing instance information for all instances in a scene.
--- @param scene string
--- @return table instances
---@diagnostic disable-next-line: missing-return
syrup.scenes.instances = function(scene)end

--- [API] Add an instance to a scene.
--- @param scene string
--- @param type string
--- @param x number
--- @param y number
--- @param z number
syrup.scenes.add_instance = function(scene, type, x, y, z)end

--- [API] Remove an instance from a scene.
--- @param scene string
--- @param index integer
syrup.scenes.delete_instance = function(scene, index)end

--- [API] Change the type of an instance in a scene.
--- @param scene string
--- @param index integer
--- @param type string
syrup.scenes.set_instance_type = function(scene, index, type)end

--- [API] Move an instance's location in a scene.
--- @param scene string
--- @param index integer
--- @param x number
--- @param y number
--- @param z number
syrup.scenes.move_instance = function(scene, index, x, y, z)end
