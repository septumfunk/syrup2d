require("ui.info")

function draw_menu(x, y, entries)
    max_width = 15
    for _, entry in pairs(entries) do
        if string.len(entry.text) * ui_text_size.width > max_width then
            max_width = string.len(entry.text) * ui_text_size.width
        end
    end

    height = ui_text_size.height + 4
    for index, entry in pairs(entries) do
        hovering = mouse.x > x and mouse.x < x + max_width and mouse.y > y + height * (index-1) and mouse.y < y + height * (index-1) + height
        if hovering then
            draw_rectangle(x, y + height * (index-1), max_width + 4, height, hover_color)
        else
            draw_rectangle(x, y + height * (index-1), max_width + 4, height, normal_color)
        end
        draw_ui_text(x + 2, y + height * (index-1) + 2, entry.text)
        if hovering and is_mouse_button_pressed(mouse_button.left) then
            entry.on_click()
        end
    end
end