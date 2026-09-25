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

#include "ccc-function-description.h"
#include <ns3/log.h>
#include <nlohmann/json.hpp>

namespace ns3 {

using json = nlohmann::json;

NS_LOG_COMPONENT_DEFINE("CccFunctionDescription");

CccFunctionDescription::CccFunctionDescription()
{
    FillAndEncodeCccFunctionDescription();
}

CccFunctionDescription::~CccFunctionDescription()
{
    if (m_buffer) {
        free(m_buffer);
        m_buffer = nullptr;
        m_size = 0;
    }
}

void
CccFunctionDescription::FillAndEncodeCccFunctionDescription()
{
    NS_LOG_FUNCTION(this);
    
    // Create the main RAN Function Definition structure
    json ranFuncDefJson;
    
    // ===== RAN Function Name =====
    ranFuncDefJson["ranFunctionName"] = {
        {"ranFunctionShortName", "E2SM-CCC"},
        {"ranFunctionServiceModelOID", "1.3.6.1.4.1.53148.1.2.2.4"},
        {"ranFunctionDescription", "Cell Configuration and Control Service Model"},
        {"ranFunctionInstance", 1}
    };
    
    // ===== List of Supported Node-Level RAN Configuration Structures =====
    json nodeLevelConfigs = json::array({
        {
            {"ranConfigurationStructureName", "O-GnbCuCpFunction"},
            {"listOfSupportedAttributes", json::array({
                {"gNBId"}, 
                {"gNBIdLength"},
                {"plmnId"}
            })}
        },
        {
            {"ranConfigurationStructureName", "O-GnbCuUpFunction"},
            {"listOfSupportedAttributes", json::array({
                {"gNBCuUpId"},
                {"plmnId"}
            })}
        },
        {
            {"ranConfigurationStructureName", "O-GnbDuFunction"},
            {"listOfSupportedAttributes", json::array({
                {"gNBDuId"},
                {"gNBDuName"},
                {"gNBIdLength"}
            })}
        }
    });
    
    ranFuncDefJson["listOfSupportedNodeLevelConfigurationStructures"] = nodeLevelConfigs;
    
    // ===== List of Cells for RAN Function Definition =====
    json cellsList = json::array({
        {
            {"cellGlobalId", {
                {"nrCgi", {
                    {"plmnIdentity", "001001"},
                    {"nrCellIdentity", "12345678"}
                }}
            }},
            {"listOfSupportedCellLevelRANConfigurationStructures", json::array({
                {
                    {"ranConfigurationStructureName", "O-NrCellDu"},
                    {"listOfSupportedAttributes", json::array({
                        {"cellLocalId"},
                        {"administrativeState"},
                        {"nRPCI"},
                        {"nRTAC"},
                        {"arfcnDL"},
                        {"arfcnUL"},
                        {"bSChannelBwDL"},
                        {"bSChannelBwUL"}
                    })}
                },
                {
                    {"ranConfigurationStructureName", "O-NrCellCu"},
                    {"listOfSupportedAttributes", json::array({
                        {"cellLocalId"},
                        {"plmnInfoList"},
                        {"nRPCI"},
                        {"nRTAC"}
                    })}
                }
            })}
        }
    });
    
    ranFuncDefJson["listOfCellsForRANFunctionDefinition"] = cellsList;
    
    // ===== RIC Services =====
    json ricServices;
    
    // ----- Event Trigger Service -----
    ricServices["eventTrigger"] = {
        {"listOfSupportedEventTriggerStyles", json::array({
            {
                {"eventTriggerStyleType", 1},
                {"eventTriggerStyleName", "Node-Level Config Change Trigger"},
                {"eventTriggerFormatType", 1}
            },
            {
                {"eventTriggerStyleType", 2},
                {"eventTriggerStyleName", "Cell-Level Config Change Trigger"},
                {"eventTriggerFormatType", 2}
            },
            {
                {"eventTriggerStyleType", 3},
                {"eventTriggerStyleName", "Periodic Report Trigger"},
                {"eventTriggerFormatType", 3}
            }
        })}
    };
    
    // ----- Report Service -----
    ricServices["reportService"] = {
        {"listOfSupportedReportStyles", json::array({
            {
                {"ricIndicationHeaderFormatType", 1},
                {"ricIndicationMessageFormatType", 1},
                {"reportStyleType", 1},
                {"reportStyleName", "Node-Level Config Report"}
            },
            {
                {"ricIndicationHeaderFormatType", 1},
                {"ricIndicationMessageFormatType", 2},
                {"reportStyleType", 2},
                {"reportStyleName", "Cell-Level Config Report"}
            }
        })}
    };
    
    // ----- Control Service (Main CCC Feature) -----
    ricServices["controlService"] = {
        {"listOfSupportedControlStyles", json::array({
            {
                {"ricControlStyleType", 1},
                {"ricControlStyleName", "Cell Power Control"},
                {"ricControlHeaderFormatType", 1},
                {"ricControlMessageFormatType", 1},
                {"ricControlOutcomeFormatType", 1},
                {"listOfControlActions", json::array({
                    {
                        {"ricControlActionId", 1},
                        {"ricControlActionName", "Transmit Power Adjustment"},
                        {"ranParameterList", json::array({
                            {
                                {"ranParameterId", 1},
                                {"ranParameterName", "txPower"},
                                {"ranParameterType", "integer"}
                            },
                            {
                                {"ranParameterId", 2},
                                {"ranParameterName", "cellId"},
                                {"ranParameterType", "integer"}
                            }
                        })}
                    }
                })}
            },
            {
                {"ricControlStyleType", 2},
                {"ricControlStyleName", "Antenna Configuration Control"},
                {"ricControlHeaderFormatType", 1},
                {"ricControlMessageFormatType", 1},
                {"ricControlOutcomeFormatType", 1},
                {"listOfControlActions", json::array({
                    {
                        {"ricControlActionId", 2},
                        {"ricControlActionName", "Antenna Port Configuration"},
                        {"ranParameterList", json::array({
                            {
                                {"ranParameterId", 3},
                                {"ranParameterName", "antennaPort"},
                                {"ranParameterType", "integer"}
                            },
                            {
                                {"ranParameterId", 4},
                                {"ranParameterName", "cellId"},
                                {"ranParameterType", "integer"}
                            },
                            {
                                {"ranParameterId", 5},
                                {"ranParameterName", "beamformingVector"},
                                {"ranParameterType", "array"}
                            }
                        })}
                    },
                    {
                        {"ricControlActionId", 3},
                        {"ricControlActionName", "Dynamic Antenna Sleep Mode"},
                        {"ranParameterList", json::array({
                            {
                                {"ranParameterId", 6},
                                {"ranParameterName", "sleepMode"},
                                {"ranParameterType", "string"}
                            },
                            {
                                {"ranParameterId", 7},
                                {"ranParameterName", "duration"},
                                {"ranParameterType", "integer"}
                            },
                            {
                                {"ranParameterId", 8},
                                {"ranParameterName", "trxControlMask"},
                                {"ranParameterType", "string"}
                            }
                        })}
                    }
                })}
            },
            {
                {"ricControlStyleType", 3},
                {"ricControlStyleName", "Cell Carrier Management"},
                {"ricControlHeaderFormatType", 1},
                {"ricControlMessageFormatType", 2},
                {"ricControlOutcomeFormatType", 2},
                {"listOfControlActions", json::array({
                    {
                        {"ricControlActionId", 4},
                        {"ricControlActionName", "Cell Activation/Deactivation"},
                        {"ranParameterList", json::array({
                            {
                                {"ranParameterId", 9},
                                {"ranParameterName", "administrativeState"},
                                {"ranParameterType", "string"}
                            },
                            {
                                {"ranParameterId", 10},
                                {"ranParameterName", "cellId"},
                                {"ranParameterType", "integer"}
                            }
                        })}
                    },
                    {
                        {"ricControlActionId", 5},
                        {"ricControlActionName", "Carrier Aggregation Control"},
                        {"ranParameterList", json::array({
                            {
                                {"ranParameterId", 11},
                                {"ranParameterName", "primaryCarrier"},
                                {"ranParameterType", "integer"}
                            },
                            {
                                {"ranParameterId", 12},
                                {"ranParameterName", "secondaryCarriers"},
                                {"ranParameterType", "array"}
                            }
                        })}
                    }
                })}
            },
            {
                {"ricControlStyleType", 4},
                {"ricControlStyleName", "Radio Resource Management"},
                {"ricControlHeaderFormatType", 1},
                {"ricControlMessageFormatType", 1},
                {"ricControlOutcomeFormatType", 1},
                {"listOfControlActions", json::array({
                    {
                        {"ricControlActionId", 6},
                        {"ricControlActionName", "PRB Allocation Control"},
                        {"ranParameterList", json::array({
                            {
                                {"ranParameterId", 13},
                                {"ranParameterName", "prbQuota"},
                                {"ranParameterType", "integer"}
                            },
                            {
                                {"ranParameterId", 14},
                                {"ranParameterName", "sliceId"},
                                {"ranParameterType", "integer"}
                            }
                        })}
                    }
                })}
            }
        })}
    };
    
    // ----- Query Service -----
    ricServices["queryService"] = {
        {"listOfSupportedQueryStyles", json::array({
            {
                {"queryStyleType", 1},
                {"queryStyleName", "Cell Configuration Query"},
                {"queryHeaderFormatType", 1},
                {"queryDefinitionFormatType", 1},
                {"queryOutcomeFormatType", 1}
            },
            {
                {"queryStyleType", 2},
                {"queryStyleName", "Multi-Cell Configuration Query"},
                {"queryHeaderFormatType", 1},
                {"queryDefinitionFormatType", 2},
                {"queryOutcomeFormatType", 2}
            }
        })}
    };
    
    ranFuncDefJson["ricServices"] = ricServices;
    
    // Serialize to JSON string
    std::string jsonString = ranFuncDefJson.dump(2);  // Pretty print with 2 spaces
    
    // Store in buffer (JSON format instead of binary ASN.1)
    m_size = jsonString.length();
    m_buffer = malloc(m_size + 1);  // +1 for null terminator
    std::memcpy(m_buffer, jsonString.c_str(), m_size);
    ((char*)m_buffer)[m_size] = '\0';  // Null terminate
    
    NS_LOG_INFO("CCC Function Description created (JSON-encoded):");
    NS_LOG_INFO("Size: " << m_size << " bytes");
    NS_LOG_INFO("Content:\n" << jsonString);
}


} // namespace ns3

