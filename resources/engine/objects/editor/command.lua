require("syrup")

return {
    depth = -999,
    _entries = {},

    start = function(self)
        self:_update_width()
    end,

    update = function(self)
        self.children_hovering = false
        if syrup.input.is_mouse_button_pressed(syrup.input.mouse_button.left) then
            for _, entry in ipairs(self._entries) do
                if entry.hovering then
                    self.children_hovering = true
                end
            end
            if not self.children_hovering then
                syrup.objects.delete(self.id)
            end
        end
    end,

    clean_up = function(self)
        for _, entry in ipairs(self._entries) do
            syrup.objects.delete(entry.id)
        end
    end,

    add_entry = function(self, text, on_click)
        local object = syrup.objects.new("editor/button", -999, -999)
        if object == nil then return end

        object:set_text(text)
        object.on_click = on_click
        table.insert(self._entries, object)

        for i, entry in ipairs(self._entries) do
            entry.x = self.x
            entry.y = self.y - entry.height * (i-1)
        end
        self:_update_width()
    end,

    _update_width = function(self)
        local longest = 0
        for _, entry in ipairs(self._entries) do
            if string.len(entry._text) > longest then
                longest = string.len(entry._text)
            end
        end
        self.width = longest * syrup.ui.font_size.width

        for _, entry in ipairs(self._entries) do
            entry:set_width(self.width)
        end
    end,
}