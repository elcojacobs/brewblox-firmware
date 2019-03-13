#include "TimerInterrupts.h"
#include "spark_wiring_interrupts.h"
#include <algorithm>
#include <functional>
#include <vector>

struct FuncEntry {
    uint8_t id;
    std::function<void()> func;
};

static std::vector<FuncEntry> funcs;

void
timerIsrHandler()
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

        for (auto& fe : funcs) {
            fe.func();
        }
    }
}

void
TimerInterrupts::init()
{

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // Enable Timer Interrupt
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM4_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 10;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);

    // Timebase configuration (10 kHz), so we can get 100 steps resolution for 100Hz PWM
    // SysCoreClock = 120 Mhz, timer clock is 60MHz, 60Mhz / 6000 = 10kHz
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 5; // divides by 6
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 1000;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM4, &timerInitStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);

    attachSystemInterrupt(SysInterrupt_TIM4_Update, timerIsrHandler);
}

int16_t
TimerInterrupts::add(std::function<void()>&& func)
{
    for (uint8_t id = 1; id < 255; id++) {
        // find free id
        auto match = find_if(funcs.begin(), funcs.end(), [&id](const FuncEntry& fe) {
            return fe.id == id;
        });

        if (match == funcs.end()) {
            funcs.push_back(FuncEntry{id, std::move(func)});
            TIM_Cmd(TIM4, ENABLE);
            return id;
        }
    };
    return 0;
}

void
TimerInterrupts::remove(uint8_t id)
{
    funcs.erase(std::remove_if(funcs.begin(), funcs.end(), [&id](const FuncEntry& fe) { return fe.id == id; }));
    if (funcs.empty()) {
        TIM_Cmd(TIM4, DISABLE);
    }
}
