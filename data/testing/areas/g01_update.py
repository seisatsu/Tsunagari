duration = 1.0
max_alpha = 192

if drinking == True:
    progress = well_timer.count / duration
    if progress < 0.5:
        overlay_alpha = int(2 * max_alpha * progress)
    elif progress < 1.0:
        overlay_alpha = int(2 * max_alpha * (1 - progress))
    else:
        overlay_alpha = 0
        drinking = False
    Area.color_overlay(255, 255, 255, overlay_alpha) # white overlay

