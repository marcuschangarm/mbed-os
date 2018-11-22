/*
 * Copyright (c) 2018, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef QUECTEL_BC95_CELLULARCONTEXT_H_
#define QUECTEL_BC95_CELLULARCONTEXT_H_

#include "AT_CellularContext.h"

namespace mbed {

class QUECTEL_BC95_CellularContext: public AT_CellularContext {
public:
    QUECTEL_BC95_CellularContext(ATHandler &at, CellularDevice *device, const char *apn);
    virtual ~QUECTEL_BC95_CellularContext();

protected:
    virtual NetworkStack *get_stack();
    virtual bool stack_type_supported(nsapi_ip_stack_t stack_type);
};

} /* namespace mbed */

#endif // QUECTEL_BC95_CELLULARCONTEXT_H_