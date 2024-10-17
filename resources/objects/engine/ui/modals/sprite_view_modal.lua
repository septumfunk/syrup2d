require("ui.info")

return {
    sprite = "default",
    frame = 0,

    start = function(this)
        this.modal = object_new("engine/ui/modals/modal", 0, 0)
        this.modal.titlebar.title = this.sprite
        this.modal.resize(this.modal, sprite_width(this.sprite), sprite_height(this.sprite))
        this.modal.cleanup = function(_)
            object_delete(this.id)
        end
        this.depth = this.modal.depth + 0.1
        this.modal.x = dimensions.width / 2 - this.modal.width / 2
        this.modal.y = dimensions.height / 2 - this.modal.height / 2
    end,

    set_sprite = function(this, sprite)
        this.sprite = sprite
        this.modal.titlebar.title = sprite
        this.modal.resize(this.modal, sprite_width(sprite), sprite_height(sprite))
    end,

    update = function(this)
        this.depth = this.modal.depth + 0.1
    end,

    draw_gui = function(this)
        draw_sprite(this.sprite, this.modal.x + this.modal.width / 2 - sprite_width(this.sprite) / 2, this.modal.y + this.modal.height / 2 - sprite_height(this.sprite) / 2, this.frame)
    end,
}