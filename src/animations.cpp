#include <Arduino.h>

#include "animations.h"
#include "devices.h"

// ========================================
// ANIMATION STATE
// ========================================

AnimationType currentAnimation = ANIMATION_NONE;

unsigned long lastAnimationTime = 0;

const unsigned long ANIMATION_INTERVAL = 250;

// Auto-light timing
const unsigned long AUTO_STEP_TIME = 400;
const unsigned long ALL_ON_TIME = 5000;

int animationStep = 0;


// ========================================
// INITIALIZE
// ========================================

void initAnimations()
{
    currentAnimation = ANIMATION_NONE;

    animationStep = 0;

    lastAnimationTime = millis();

    setLED1(false);
    setLED2(false);
    setLED3(false);

    Serial.println("Animation system initialized");
}


// ========================================
// SET ANIMATION
// ========================================

void setAnimation(AnimationType animation)
{
    if (currentAnimation != animation)
    {
        currentAnimation = animation;

        animationStep = 0;

        lastAnimationTime = millis();

        setLED1(false);
        setLED2(false);
        setLED3(false);
    }
}


// ========================================
// AUTO LIGHT ANIMATION
// ========================================
//
// ASCENDING:
//
// ●○○
// ●●○
// ●●●
//
// HOLD:
//
// ●●●
// 5 seconds
//
// DESCENDING:
//
// ○●●
// ○○●
// ○○○
//
// ========================================

void updateAutoAnimation()
{
    unsigned long now = millis();

    // ------------------------------------
    // Steps 0,1,2,4,5,6
    // ------------------------------------

    if (animationStep != 3)
    {
        if (now - lastAnimationTime < AUTO_STEP_TIME)
            return;
    }

    switch (animationStep)
    {
        case 0:
            // ●○○
            setLED1(true);
            setLED2(false);
            setLED3(false);

            animationStep = 1;
            lastAnimationTime = now;
            break;


        case 1:
            // ●●○
            setLED1(true);
            setLED2(true);
            setLED3(false);

            animationStep = 2;
            lastAnimationTime = now;
            break;


        case 2:
            // ●●●
            setLED1(true);
            setLED2(true);
            setLED3(true);

            animationStep = 3;
            lastAnimationTime = now;
            break;


        case 3:
            // --------------------------------
            // ALL LEDs ON FOR 5 SECONDS
            // --------------------------------

            setLED1(true);
            setLED2(true);
            setLED3(true);

            if (now - lastAnimationTime >= ALL_ON_TIME)
            {
                animationStep = 4;
                lastAnimationTime = now;
            }

            break;


        case 4:
            // ○●●
            setLED1(false);
            setLED2(true);
            setLED3(true);

            animationStep = 5;
            lastAnimationTime = now;
            break;


        case 5:
            // ○○●
            setLED1(false);
            setLED2(false);
            setLED3(true);

            animationStep = 6;
            lastAnimationTime = now;
            break;


        case 6:
            // ○○○
            setLED1(false);
            setLED2(false);
            setLED3(false);

            animationStep = 0;
            lastAnimationTime = now;
            break;
    }
}


// ========================================
// PIR SECURITY ANIMATION
// ========================================

void updatePIRAnimation()
{
    if (animationStep == 0)
    {
        // ●○●
        setLED1(true);
        setLED2(false);
        setLED3(true);
    }
    else
    {
        // ○●○
        setLED1(false);
        setLED2(true);
        setLED3(false);
    }

    animationStep++;

    if (animationStep >= 2)
        animationStep = 0;
}


// ========================================
// FLAME / FIRE ANIMATION
// ========================================

void updateFlameAnimation()
{
    switch (animationStep)
    {
        case 0:
            // ●●●
            setLED1(true);
            setLED2(true);
            setLED3(true);
            break;

        case 1:
            // ○○○
            setLED1(false);
            setLED2(false);
            setLED3(false);
            break;

        case 2:
            // ●○●
            setLED1(true);
            setLED2(false);
            setLED3(true);
            break;

        case 3:
            // ○○○
            setLED1(false);
            setLED2(false);
            setLED3(false);
            break;
    }

    animationStep++;

    if (animationStep >= 4)
        animationStep = 0;
}


// ========================================
// UPDATE ANIMATIONS
// ========================================

void updateAnimations()
{
    if (currentAnimation == ANIMATION_NONE)
        return;

    // ------------------------------------
    // AUTO LIGHT
    // ------------------------------------

    if (currentAnimation == ANIMATION_AUTO_LIGHT)
    {
        updateAutoAnimation();
        return;
    }

    // ------------------------------------
    // ALARM ANIMATIONS
    // ------------------------------------

    if (millis() - lastAnimationTime < ANIMATION_INTERVAL)
        return;

    lastAnimationTime = millis();

    if (currentAnimation == ANIMATION_PIR_ALARM)
    {
        updatePIRAnimation();
    }
    else if (currentAnimation == ANIMATION_FLAME_ALARM)
    {
        updateFlameAnimation();
    }
}


// ========================================
// GET CURRENT ANIMATION
// ========================================

AnimationType getAnimation()
{
    return currentAnimation;
}