/*
 * Copyright (c) 2019-2021 Piotr Stolarz
 * OneWireNg: Ardiono 1-wire service library
 *
 * Distributed under the 2-clause BSD License (the License)
 * see accompanying file LICENSE for details.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the License for more information.
 */

#ifndef __OWNG_ARDUINO_MEGAAVR__
#define __OWNG_ARDUINO_MEGAAVR__

#include <assert.h>
#include "Arduino.h"
#include "OneWireNg_BitBang.h"

#define __READ_GPIO(gs) \
    ((gs.port->IN & gs.bmsk) != 0)

#define __WRITE_GPIO(gs, st) \
    if (st) gs.port->OUTSET = gs.bmsk; \
    else gs.port->OUTCLR = gs.bmsk

#define __GPIO_AS_INPUT(gs) \
    gs.port->DIRCLR = gs.bmsk

#define __GPIO_AS_OUTPUT(gs) \
    gs.port->DIRSET = gs.bmsk

/**
 * Arduino megaAVR platform GPIO specific implementation
 * (this is recent Microchip architecture: ATmega4809, 4808, 3209, 3208).
 */
class OneWireNg_ArduinoMegaAVR: public OneWireNg_BitBang
{
public:
    /**
     * OneWireNg 1-wire service for Arduino megaAVR platform.
     *
     * Bus powering is supported via switching its GPIO to the high state.
     * In this case the GPIO servers as a voltage source for connected slaves
     * working in parasite powering configuration.
     *
     * @param pin Arduino GPIO pin number used for bit-banging 1-wire bus.
     * @param pullUp If @c true configure internal pull-up resistor for the bus.
     */
    OneWireNg_ArduinoMegaAVR(unsigned pin, bool pullUp):
        OneWireNg_BitBang(false)
    {
        initDtaGpio(pin, pullUp);
    }

    /**
     * OneWireNg 1-wire service for Arduino megaAVR platform.
     *
     * Bus powering is supported via a switching transistor providing
     * the power to the bus and controlled by a dedicated GPIO (@see
     * OneWireNg_BitBang::setupPwrCtrlGpio()). In this configuration the
     * service mimics the open-drain type of output. The approach may be
     * feasible if the GPIO is unable to provide sufficient power for
     * connected slaves working in parasite powering configuration.
     *
     * @param pin Arduino GPIO pin number used for bit-banging 1-wire bus.
     * @param pwrCtrlPin Arduino GPIO pin number controlling the switching
     *     transistor.
     * @param pullUp If @c true configure internal pull-up resistor for the bus.
     */
    OneWireNg_ArduinoMegaAVR(unsigned pin, unsigned pwrCtrlPin, bool pullUp):
        OneWireNg_BitBang(true)
    {
        initDtaGpio(pin, pullUp);
        initPwrCtrlGpio(pwrCtrlPin);
    }

protected:
    int readGpioIn(GpioType gpio)
    {
        UNUSED(gpio);
        return __READ_GPIO(_dtaGpio);
    }

    void writeGpioOut(GpioType gpio, int state)
    {
        if (gpio == GPIO_DTA) {
            __WRITE_GPIO(_dtaGpio, state);
        } else {
            __WRITE_GPIO(_pwrCtrlGpio, state);
        }
    }

    void setGpioAsInput(GpioType gpio)
    {
        UNUSED(gpio);
        __GPIO_AS_INPUT(_dtaGpio);
    }

    void setGpioAsOutput(GpioType gpio, int state)
    {
        if (gpio == GPIO_DTA) {
            __WRITE_GPIO(_dtaGpio, state);
            __GPIO_AS_OUTPUT(_dtaGpio);
        } else {
            __WRITE_GPIO(_pwrCtrlGpio, state);
            __GPIO_AS_OUTPUT(_pwrCtrlGpio);
        }
    }

    void initDtaGpio(unsigned pin, bool pullUp)
    {
        _dtaGpio.bmsk = digitalPinToBitMask(pin);
        _dtaGpio.port = digitalPinToPortStruct(pin);
        volatile uint8_t *ctrlReg = getPINnCTRLregister(_dtaGpio.port,
            digitalPinToBitPosition(pin));

        assert(_dtaGpio.port != NULL && ctrlReg != NULL);

        __GPIO_AS_INPUT(_dtaGpio);

        /* non-inverting mode */
        *ctrlReg &= ~(PORT_INVEN_bm);

        if (pullUp) {
            *ctrlReg |= PORT_PULLUPEN_bm;
        } else {
            *ctrlReg &= ~(PORT_PULLUPEN_bm);
        }

        setupDtaGpio();
    }

    void initPwrCtrlGpio(unsigned pin)
    {
        _pwrCtrlGpio.bmsk = digitalPinToBitMask(pin);
        _pwrCtrlGpio.port = digitalPinToPortStruct(pin);
        volatile uint8_t *ctrlReg = getPINnCTRLregister(_pwrCtrlGpio.port,
            digitalPinToBitPosition(pin));

        assert(_pwrCtrlGpio.port != NULL && ctrlReg != NULL);

        /* non-inverting mode */
        *ctrlReg &= ~(PORT_INVEN_bm);

        setupPwrCtrlGpio(true);
    }

    struct {
        uint8_t bmsk;
        volatile PORT_t *port;
    } _dtaGpio;

    struct {
        uint8_t bmsk;
        volatile PORT_t *port;
    } _pwrCtrlGpio;
};

#undef __GPIO_AS_OUTPUT
#undef __GPIO_AS_INPUT
#undef __WRITE_GPIO
#undef __READ_GPIO

#endif /* __OWNG_ARDUINO_MEGAAVR__ */
