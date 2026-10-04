#pragma once

enum AnimationType
{
    ANIMATION_NONE,
    ANIMATION_AUTO_LIGHT,
    ANIMATION_PIR_ALARM,
    ANIMATION_FLAME_ALARM
};

void initAnimations();
void updateAnimations();

void setAnimation(AnimationType animation);
AnimationType getAnimation();