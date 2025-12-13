/**
 * @file lcd_advanced_demo.c
 * @brief Advanced LCD demo - optimized for N32G430 32KB RAM
 * @note Due to RAM constraints, using direct LCD writes instead of full framebuffer
 */

#include "lcd_advanced_demo.h"
#include "lcd_init.h"
#include "lcd.h"
#include "spi.h"
#include "dma.h"
#include "hal_compat.h"
#include <stdlib.h>
#include <stdio.h>

/* Demo states */
typedef enum {
    DEMO_BOUNCING_BALLS,
    DEMO_PLASMA_EFFECT,
    DEMO_PARTICLES,
    DEMO_GRAPHICS_TEST,
    DEMO_PERFORMANCE,
    DEMO_COUNT
} DemoState_t;

/* Particle system */
#define MAX_PARTICLES 30
typedef struct {
    float x, y;
    float vx, vy;
    uint16_t color;
    uint8_t life;
    bool active;
    /* Previous position for erasing */
    float prev_x, prev_y;
} Particle_t;

/* Bouncing ball */
#define MAX_BALLS 5
typedef struct {
    float x, y;
    float vx, vy;
    uint8_t radius;
    uint16_t color;
    /* Previous position for erasing */
    float prev_x, prev_y;
} Ball_t;

/* Demo context */
static struct {
    DemoState_t state;
    uint32_t last_switch_time;
    uint32_t frame_count;
    uint32_t fps;
    uint32_t last_fps_time;

    /* Bouncing balls */
    Ball_t balls[MAX_BALLS];
    bool balls_initialized;

    /* Particles */
    Particle_t particles[MAX_PARTICLES];
    uint32_t particle_spawn_timer;
    bool particles_initialized;

    /* Plasma effect */
    uint32_t plasma_time;

    /* Graphics test */
    bool graphics_initialized;

} demo_ctx;

/* Color palette for effects */
static const uint16_t rainbow_colors[] = {
    RED, 0xFC00, YELLOW, GREEN,    // RED, ORANGE, YELLOW, GREEN
    CYAN, BLUE, 0x8010, MAGENTA    // CYAN, BLUE, PURPLE, MAGENTA
};
#define RAINBOW_SIZE (sizeof(rainbow_colors) / sizeof(rainbow_colors[0]))

/* Helper: Generate random color */
static uint16_t random_color(void) {
    return rainbow_colors[rand() % RAINBOW_SIZE];
}

/* Helper: Generate random float between min and max */
static float randf(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

/* Helper: Fast sine approximation (Taylor series) */
static float fast_sin(float x) {
    while (x > 3.14159f) x -= 6.28318f;
    while (x < -3.14159f) x += 6.28318f;
    return x - (x*x*x)/6.0f + (x*x*x*x*x)/120.0f;
}

/* Helper: Fast cosine approximation */
static float fast_cos(float x) {
    return fast_sin(x + 1.5708f);
}

/**
 * @brief Initialize bouncing balls
 */
static void init_balls(void) {
    for (int i = 0; i < MAX_BALLS; i++) {
        demo_ctx.balls[i].x = randf(10, LCD_W - 10);
        demo_ctx.balls[i].y = randf(10, LCD_H - 10);
        demo_ctx.balls[i].prev_x = demo_ctx.balls[i].x;
        demo_ctx.balls[i].prev_y = demo_ctx.balls[i].y;
        demo_ctx.balls[i].vx = randf(-2.0f, 2.0f);
        demo_ctx.balls[i].vy = randf(-2.0f, 2.0f);
        demo_ctx.balls[i].radius = 5 + (i * 2);
        demo_ctx.balls[i].color = rainbow_colors[i % RAINBOW_SIZE];
    }
}

/**
 * @brief Update and draw bouncing balls with partial refresh
 */
static void demo_bouncing_balls(void) {
    /* Clear screen and draw title only once */
    if (!demo_ctx.balls_initialized) {
        LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
        LCD_ShowString(5, 5, (const u8 *)"Bouncing Balls", WHITE, BLACK, 12, 0);
        demo_ctx.balls_initialized = true;
    }

    for (int i = 0; i < MAX_BALLS; i++) {
        Ball_t *ball = &demo_ctx.balls[i];

        /* Erase old position */
        Draw_Circle((uint16_t)ball->prev_x, (uint16_t)ball->prev_y, ball->radius + 1, BLACK);

        /* Save current position */
        ball->prev_x = ball->x;
        ball->prev_y = ball->y;

        /* Update position */
        ball->x += ball->vx;
        ball->y += ball->vy;

        /* Bounce off walls */
        if (ball->x - ball->radius < 0 || ball->x + ball->radius >= LCD_W) {
            ball->vx = -ball->vx;
            ball->x = (ball->x < LCD_W / 2) ? ball->radius : LCD_W - ball->radius - 1;
        }
        if (ball->y - ball->radius < 0 || ball->y + ball->radius >= LCD_H) {
            ball->vy = -ball->vy;
            ball->y = (ball->y < LCD_H / 2) ? ball->radius : LCD_H - ball->radius - 1;
        }

        /* Draw ball at new position */
        Draw_Circle((uint16_t)ball->x, (uint16_t)ball->y, ball->radius, ball->color);
    }
}

/**
 * @brief Plasma effect demo (simplified for performance)
 */
static void demo_plasma_effect(void) {
    demo_ctx.plasma_time++;

    /* Draw simplified plasma pattern */
    for (uint16_t y = 0; y < LCD_H; y += 2) {
        for (uint16_t x = 0; x < LCD_W; x += 2) {
            float cx = (x - LCD_W/2) / 20.0f;
            float cy = (y - LCD_H/2) / 20.0f;

            float v = fast_sin(cx + demo_ctx.plasma_time * 0.1f) +
                      fast_sin(cy + demo_ctx.plasma_time * 0.08f);

            int color_idx = ((int)(v * 2.0f) + demo_ctx.plasma_time / 10) % RAINBOW_SIZE;
            if (color_idx < 0) color_idx += RAINBOW_SIZE;

            LCD_DrawPoint(x, y, rainbow_colors[color_idx]);
        }
    }

    LCD_ShowString(5, 5, (const u8 *)"Plasma Effect", WHITE, BLACK, 12, 0);
}

/**
 * @brief Initialize particle system
 */
static void init_particles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        demo_ctx.particles[i].active = false;
    }
    demo_ctx.particle_spawn_timer = 0;
}

/**
 * @brief Spawn a new particle
 */
static void spawn_particle(float x, float y) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!demo_ctx.particles[i].active) {
            Particle_t *p = &demo_ctx.particles[i];
            p->x = x;
            p->y = y;
            p->prev_x = x;
            p->prev_y = y;
            p->vx = randf(-3.0f, 3.0f);
            p->vy = randf(-4.0f, -1.0f);
            p->color = random_color();
            p->life = 60;
            p->active = true;
            break;
        }
    }
}

/**
 * @brief Particle effect demo with partial refresh
 */
static void demo_particles(void) {
    /* Clear screen only once */
    if (!demo_ctx.particles_initialized) {
        LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
        LCD_ShowString(5, 5, (const u8 *)"Particle Fountain", WHITE, BLACK, 12, 0);
        demo_ctx.particles_initialized = true;
    }

    /* Spawn particles from bottom center */
    demo_ctx.particle_spawn_timer++;
    if (demo_ctx.particle_spawn_timer >= 3) {
        spawn_particle(LCD_W / 2.0f, LCD_H - 5.0f);
        demo_ctx.particle_spawn_timer = 0;
    }

    /* Update and draw particles */
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle_t *p = &demo_ctx.particles[i];
        if (!p->active) continue;

        /* Erase old position */
        if (p->prev_x >= 0 && p->prev_x < LCD_W && p->prev_y >= 0 && p->prev_y < LCD_H) {
            LCD_DrawPoint((uint16_t)p->prev_x, (uint16_t)p->prev_y, BLACK);
            if (p->life > 10) {
                LCD_DrawPoint((uint16_t)p->prev_x + 1, (uint16_t)p->prev_y, BLACK);
                LCD_DrawPoint((uint16_t)p->prev_x, (uint16_t)p->prev_y + 1, BLACK);
            }
        }

        /* Save old position */
        p->prev_x = p->x;
        p->prev_y = p->y;

        /* Update physics */
        p->vy += 0.2f;  /* Gravity */
        p->x += p->vx;
        p->y += p->vy;
        p->life--;

        if (p->life == 0 || p->y >= LCD_H) {
            p->active = false;
            continue;
        }

        /* Draw particle at new position */
        if (p->x >= 0 && p->x < LCD_W && p->y >= 0 && p->y < LCD_H) {
            LCD_DrawPoint((uint16_t)p->x, (uint16_t)p->y, p->color);
            if (p->life > 10) {
                LCD_DrawPoint((uint16_t)p->x + 1, (uint16_t)p->y, p->color);
                LCD_DrawPoint((uint16_t)p->x, (uint16_t)p->y + 1, p->color);
            }
        }
    }
}

/**
 * @brief Graphics benchmark with partial refresh
 */
static void demo_graphics_test(void) {
    static uint32_t anim_frame = 0;
    static uint16_t prev_rect_x[5] = {0};
    static uint16_t prev_line_x2[8] = {0};
    static uint16_t prev_line_y2[8] = {0};
    static uint8_t prev_circle_r[3] = {0};

    /* Clear screen only once */
    if (!demo_ctx.graphics_initialized) {
        LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
        LCD_ShowString(5, 5, (const u8 *)"Graphics Test", WHITE, BLACK, 12, 0);
        demo_ctx.graphics_initialized = true;
        anim_frame = 0;  /* Reset animation */
    }

    anim_frame++;

    /* Moving rectangles - erase old, draw new */
    for (int i = 0; i < 5; i++) {
        /* Erase old rectangle */
        if (anim_frame > 1) {
            LCD_Fill(prev_rect_x[i], 10 + i * 15, prev_rect_x[i] + 15, 20 + i * 15, BLACK);
        }

        /* Draw new rectangle */
        uint16_t x = (anim_frame * 2 + i * 20) % (LCD_W - 15);
        LCD_Fill(x, 10 + i * 15, x + 15, 20 + i * 15, rainbow_colors[i]);
        prev_rect_x[i] = x;
    }

    /* Rotating lines - erase old, draw new */
    for (int i = 0; i < 8; i++) {
        /* Erase old line */
        if (anim_frame > 1) {
            LCD_DrawLine(LCD_W / 2, 80, prev_line_x2[i], prev_line_y2[i], BLACK);
        }

        /* Draw new line */
        float angle = (anim_frame * 2 + i * 45) * 0.01745f;
        int x2 = LCD_W / 2 + (int)(fast_cos(angle) * 30);
        int y2 = 80 + (int)(fast_sin(angle) * 30);
        LCD_DrawLine(LCD_W / 2, 80, x2, y2, rainbow_colors[i]);
        prev_line_x2[i] = x2;
        prev_line_y2[i] = y2;
    }

    /* Growing circles - erase old, draw new */
    for (int i = 0; i < 3; i++) {
        /* Erase old circle */
        if (anim_frame > 1 && prev_circle_r[i] > 0) {
            Draw_Circle(30 + i * 35, 130, prev_circle_r[i], BLACK);
        }

        /* Draw new circle */
        uint8_t r = ((anim_frame + i * 20) % 40) / 2;
        Draw_Circle(30 + i * 35, 130, r, rainbow_colors[i * 2]);
        prev_circle_r[i] = r;
    }
}

/**
 * @brief Performance metrics display with partial refresh
 */
static void demo_performance(void) {
    static uint32_t last_update = 0;
    static uint32_t prev_fps = 0;
    static uint16_t prev_bar_width = 0;
    static bool initialized = false;
    uint32_t current_time = HAL_GetTick();

    /* Initialize only once */
    if (!initialized) {
        LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
        LCD_ShowString(10, 10, (const u8 *)"Performance Test", YELLOW, BLACK, 16, 0);
        LCD_ShowString(10, 60, (const u8 *)"LCD: 128x160", CYAN, BLACK, 12, 0);
        LCD_ShowString(10, 80, (const u8 *)"SPI DMA: ON", MAGENTA, BLACK, 12, 0);
        LCD_ShowString(10, 100, (const u8 *)"Partial Refresh", MAGENTA, BLACK, 12, 0);
        initialized = true;
        last_update = current_time;
    }

    /* Update FPS display only every 500ms to reduce overhead */
    if (current_time - last_update >= 500 && demo_ctx.fps != prev_fps) {
        char fps_str[32];

        /* Erase old FPS text area */
        LCD_Fill(10, 35, 120, 50, BLACK);

        /* Draw new FPS */
        sprintf(fps_str, "FPS: %lu", demo_ctx.fps);
        LCD_ShowString(10, 35, (const u8 *)fps_str, GREEN, BLACK, 16, 0);

        prev_fps = demo_ctx.fps;
        last_update = current_time;
    }

    /* Animated progress bar - update every 3 frames to reduce overhead */
    static uint32_t bar_anim = 0;
    static uint8_t frame_skip = 0;

    frame_skip++;
    if (frame_skip >= 3) {
        frame_skip = 0;
        bar_anim++;

        uint16_t bar_width = (bar_anim % 100) * (LCD_W - 20) / 100;

        /* Only redraw bar if width changed significantly */
        if (bar_width != prev_bar_width) {
            /* Erase old bar end */
            if (prev_bar_width > bar_width) {
                LCD_Fill(10 + bar_width, 120, 10 + prev_bar_width, 130, GRAYBLUE);
            }
            /* Draw new bar portion */
            LCD_Fill(10 + prev_bar_width, 120, 10 + bar_width, 130, GREEN);
            prev_bar_width = bar_width;

            /* Redraw background when bar resets */
            if (bar_width < 5 && prev_bar_width > 90) {
                LCD_Fill(10, 120, LCD_W - 10, 130, GRAYBLUE);
            }
        }
    }
}

/**
 * @brief Initialize advanced demo
 */
void LCD_AdvancedDemo_Init(void)
{
    /* Initialize DMA for SPI1 */
    MX_DMA_SPI1_Init();

    /* Initialize SPI1 */
    MX_SPI1_Init();

    /* Initialize LCD hardware */
    LCD_Init();

    /* Clear screen */
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

    /* Welcome message */
    LCD_ShowString(20, 60, (const u8 *)"N32G430", CYAN, BLACK, 24, 0);
    LCD_ShowString(15, 90, (const u8 *)"Advanced Demo", YELLOW, BLACK, 16, 0);
    LCD_ShowString(35, 115, (const u8 *)"SPI + DMA", WHITE, BLACK, 12, 0);

    HAL_Delay(2000);

    /* Initialize demo state */
    demo_ctx.state = DEMO_BOUNCING_BALLS;
    demo_ctx.last_switch_time = HAL_GetTick();
    demo_ctx.frame_count = 0;
    demo_ctx.fps = 0;
    demo_ctx.last_fps_time = HAL_GetTick();

    /* Initialize sub-demos */
    init_balls();
    init_particles();
    demo_ctx.plasma_time = 0;
}

/**
 * @brief Main demo loop
 */
void LCD_AdvancedDemo_Main(void)
{
    static DemoState_t prev_state = DEMO_COUNT;  /* Track state changes */
    uint32_t current_time = HAL_GetTick();

    /* Update FPS counter */
    demo_ctx.frame_count++;
    if (current_time - demo_ctx.last_fps_time >= 1000) {
        demo_ctx.fps = demo_ctx.frame_count;
        demo_ctx.frame_count = 0;
        demo_ctx.last_fps_time = current_time;
    }

    /* Switch demo every 5 seconds */
    if (current_time - demo_ctx.last_switch_time >= 5000) {
        demo_ctx.last_switch_time = current_time;
        demo_ctx.state = (DemoState_t)((demo_ctx.state + 1) % DEMO_COUNT);
    }

    /* Detect state change and reset flags */
    if (demo_ctx.state != prev_state) {
        prev_state = demo_ctx.state;

        /* Reset initialization flags */
        demo_ctx.balls_initialized = false;
        demo_ctx.particles_initialized = false;
        demo_ctx.graphics_initialized = false;

        /* Re-initialize when switching */
        if (demo_ctx.state == DEMO_BOUNCING_BALLS) {
            init_balls();
        } else if (demo_ctx.state == DEMO_PARTICLES) {
            init_particles();
        } else if (demo_ctx.state == DEMO_PLASMA_EFFECT) {
            /* Clear screen for plasma */
            LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
        }
    }

    /* Run current demo */
    switch (demo_ctx.state) {
        case DEMO_BOUNCING_BALLS:
            demo_bouncing_balls();
            break;

        case DEMO_PLASMA_EFFECT:
            /* Plasma needs full redraw */
            demo_plasma_effect();
            break;

        case DEMO_PARTICLES:
            demo_particles();
            break;

        case DEMO_GRAPHICS_TEST:
            demo_graphics_test();
            break;

        case DEMO_PERFORMANCE:
            demo_performance();
            break;

        default:
            demo_ctx.state = DEMO_BOUNCING_BALLS;
            break;
    }

    /* No delay - run as fast as possible for max FPS */
}
