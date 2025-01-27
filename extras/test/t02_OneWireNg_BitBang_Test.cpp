/*
 * Copyright (c) 2019,2021 Piotr Stolarz
 * OneWireNg: Ardiono 1-wire service library
 *
 * Distributed under the 2-clause BSD License (the License)
 * see accompanying file LICENSE for details.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the License for more information.
 */

#include "common.h"

class OneWireNg_BitBang_Test: OneWireNg_BitBang
{
private:
    OneWireNg_BitBang_Test(): OneWireNg_BitBang(false) {}

    int readGpioIn(GpioType gpio) {
        return 1;
    }

    void writeGpioOut(GpioType gpio, int state) {}

    void setGpioAsInput(GpioType gpio) {}

    void setGpioAsOutput(GpioType gpio, int state) {}
};

int main(void)
{
    return 0;
}
