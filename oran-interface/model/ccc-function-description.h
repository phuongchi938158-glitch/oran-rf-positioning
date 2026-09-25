/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Northeastern University
 * Copyright (c) 2022 Sapienza, University of Rome
 * Copyright (c) 2022 University of Padova
 * Copyright (c) 2024 Orange Innovation Egypt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: 
 *         Abdelrhman Soliman <abdelrhman.soliman.ext@orange.com>
 */

#ifndef CCC_FUNCTION_DESCRIPTION_H
#define CCC_FUNCTION_DESCRIPTION_H

#include "ns3/function-description.h"
#include "ns3/object.h"
#include <e2sim/e2sm_ccc.hpp>
#include <nlohmann/json.hpp>

namespace ns3 {

namespace quicktype {
    class RanFunctionDefinition;
}

class CccFunctionDescription : public FunctionDescription
{
public:
    CccFunctionDescription();
    ~CccFunctionDescription();

private:
    void FillAndEncodeCccFunctionDescription();
};

} // namespace ns3

#endif /* CCC_FUNCTION_DESCRIPTION_H */