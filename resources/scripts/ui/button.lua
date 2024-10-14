require("ui.info")
require("ui.text")
require("engine.mouse")

button_height = ui_text_size.height + 4

function draw_button(text, x, y, normal_color, hover_color, callback)
    width = (ui_text_size.width * string.len(text)) + 4
    hovering = mouse.x > x and mouse.x < x + width and mouse.y > y and mouse.y < y + button_height

    if (hovering) then
        draw_rectangle(x, y, width, button_height, hover_color)
    else
        draw_rectangle(x, y, width, button_height, normal_color)
    end
    draw_ui_text(x + 2, y + 2, text)

    if is_mouse_button_pressed(mouse_button.left) and hovering then
        callback()
    end
end