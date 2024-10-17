require("ui.info")

function draw_ui_text(x, y, text, color)
    draw_text(ui_text_sprite_name, x, y, string.upper(text), color)
end

function draw_ui_text_centered(x, y, text, color)
    draw_text(ui_text_sprite_name, x - string.len(text) * ui_text_size.width / 2, y - ui_text_size.height / 2, string.upper(text), color)
end