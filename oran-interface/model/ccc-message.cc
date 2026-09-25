/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2025 Orange Innovation
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
 */

 #include <assert.h>
 #include <iostream>
 #include <string>
 #include <ns3/ccc-message.h>
 #include <ns3/asn1c-types.h>
 #include <ns3/log.h>
 
 namespace ns3 {
 
 NS_LOG_COMPONENT_DEFINE ("CccControlMessage");
 
 CccControlMessage::CccControlMessage (E2AP_PDU_t* pdu)
   : m_ricStyleType(1),
     m_controlHeaderFormat(FORMAT_1_CCC_CTRL_HDR),
     m_controlMessageFormat(FORMAT_2_CCC_CTRL_MSG)
 {
   NS_LOG_FUNCTION(this);
   NS_LOG_INFO("Start of CccControlMessage::CccControlMessage()");
   DecodeRicControlMessage (pdu);
   NS_LOG_INFO("End of CccControlMessage::CccControlMessage()");
 }
 
 CccControlMessage::~CccControlMessage ()
 {
   NS_LOG_FUNCTION(this);
 }
 
 void  
 CccControlMessage::DecodeRicControlMessage(E2AP_PDU_t* pdu)
 {
   NS_LOG_FUNCTION(this);
   
   InitiatingMessage_t* mess = pdu->choice.initiatingMessage;
   auto *request = (RICcontrolRequest_t *) &mess->value.choice.RICcontrolRequest;
   
   NS_LOG_INFO("Decoding CCC RIC Control Request");
   NS_LOG_INFO (xer_fprint(stderr, &asn_DEF_RICcontrolRequest, request));
 
   size_t count = request->protocolIEs.list.count; 
   if (count <= 0) {
       NS_LOG_ERROR("[E2SM-CCC] Received empty list");
       return;
   }
 
   for (size_t i = 0; i < count; i++) 
   {
       RICcontrolRequest_IEs_t *ie = request->protocolIEs.list.array [i];
       
       switch (ie->value.present) 
       {
           case RICcontrolRequest_IEs__value_PR_RICrequestID: 
           {
               NS_LOG_DEBUG("[E2SM-CCC] Processing RICrequestID");
               m_ricRequestId = ie->value.choice.RICrequestID;
               NS_LOG_DEBUG("[E2SM-CCC] RIC Requestor ID: " << m_ricRequestId.ricRequestorID);
               NS_LOG_DEBUG("[E2SM-CCC] RIC Instance ID: " << m_ricRequestId.ricInstanceID);
               break;
           }
           
           case RICcontrolRequest_IEs__value_PR_RANfunctionID: 
           {
               m_ranFunctionId = ie->value.choice.RANfunctionID;
               NS_LOG_DEBUG("[E2SM-CCC] RAN Function ID: " << m_ranFunctionId);
               break;
           }
           
           case RICcontrolRequest_IEs__value_PR_RICcallProcessID: 
           {
               m_ricCallProcessId = ie->value.choice.RICcallProcessID;
               NS_LOG_DEBUG("[E2SM-CCC] RIC Call Process ID received");
               break;
           }
           
           case RICcontrolRequest_IEs__value_PR_RICcontrolHeader: 
           {
               NS_LOG_DEBUG("[E2SM-CCC] Processing RICcontrolHeader (JSON format)");
               
               uint8_t* headerBuffer = ie->value.choice.RICcontrolHeader.buf;
               size_t headerSize = ie->value.choice.RICcontrolHeader.size;
               
               if (headerBuffer && headerSize > 0)
               {
                   DecodeCccControlHeader(headerBuffer, headerSize);
               }
               else
               {
                   NS_LOG_ERROR("[E2SM-CCC] Empty control header");
               }
               break;
           }
           
           case RICcontrolRequest_IEs__value_PR_RICcontrolMessage: 
           {
               NS_LOG_DEBUG("[E2SM-CCC] Processing RICcontrolMessage (JSON format)");
               
               uint8_t* messageBuffer = ie->value.choice.RICcontrolMessage.buf;
               size_t messageSize = ie->value.choice.RICcontrolMessage.size;
               
               if (messageBuffer && messageSize > 0)
               {
                   DecodeCccControlMessage(messageBuffer, messageSize);
               }
               else
               {
                   NS_LOG_ERROR("[E2SM-CCC] Empty control message");
               }
               break;
           }
           
           default:
               NS_LOG_DEBUG("[E2SM-CCC] Unhandled IE present value: " << ie->value.present);
               break;
       }
  }
  
  NS_LOG_INFO("[E2SM-CCC] Control message decoded successfully");

}
 
 void
 CccControlMessage::DecodeCccControlHeader(uint8_t* headerBuffer, size_t headerSize)
 {
   NS_LOG_FUNCTION(this << headerSize);
   
   try
   {
       // Convert buffer to string
       std::string headerString(reinterpret_cast<char*>(headerBuffer), headerSize);
       NS_LOG_DEBUG("[E2SM-CCC] Control Header JSON string: " << headerString);
       
       // Parse JSON
       m_controlHeader = json::parse(headerString);
       
      // Determine control header format (currently only Format 1 is supported)
      if (m_controlHeader.contains("format"))
      {
          m_controlHeaderFormat = static_cast<ControlHeaderFormat>(
              m_controlHeader["format"].get<int>()
          );
      }
      
      // E2SM-CCC Control Header Format 1: Contains ric_style_type nested in format1
      if (m_controlHeader.contains("format1") && 
          m_controlHeader["format1"].contains("ric_style_type"))
      {
          m_ricStyleType = m_controlHeader["format1"]["ric_style_type"].get<uint32_t>();
          NS_LOG_INFO("[E2SM-CCC] RIC Style Type extracted from format1: " << m_ricStyleType);
      }
      // Fallback: check if ric_style_type is at root level (for compatibility)
      else if (m_controlHeader.contains("ric_style_type"))
      {
          m_ricStyleType = m_controlHeader["ric_style_type"].get<uint32_t>();
          NS_LOG_INFO("[E2SM-CCC] RIC Style Type extracted from root: " << m_ricStyleType);
      }
       
       NS_LOG_DEBUG("[E2SM-CCC] Control header decoded successfully");
   }
   catch (const json::exception& e)
   {
       NS_LOG_ERROR("[E2SM-CCC] JSON parsing error in control header: " << e.what());
       NS_LOG_ERROR("[E2SM-CCC] Header content: " << 
                    std::string(reinterpret_cast<char*>(headerBuffer), 
                    std::min(headerSize, size_t(100))));
   }
 }
 
 void
 CccControlMessage::DecodeCccControlMessage(uint8_t* messageBuffer, size_t messageSize)
 {
   NS_LOG_FUNCTION(this << messageSize);
   
   try
   {
       // Convert buffer to string
       std::string messageString(reinterpret_cast<char*>(messageBuffer), messageSize);
       NS_LOG_DEBUG("[E2SM-CCC] Control Message JSON string: " << messageString);
       
       // Parse JSON
       m_controlMessage = json::parse(messageString);
       
       // Determine control message format
       if (m_controlMessage.contains("format"))
       {
           m_controlMessageFormat = static_cast<ControlMessageFormat>(
               m_controlMessage["format"].get<int>()
           );
           NS_LOG_INFO("[E2SM-CCC] Control Message Format: " << m_controlMessageFormat);
       }
       
       // Parse based on format
       if (m_controlMessageFormat == FORMAT_2_CCC_CTRL_MSG)
       {
           // Format 2: List of Cells Controlled
           ParseCellControlInfo();
           NS_LOG_INFO("[E2SM-CCC] Parsed " << m_cellsControlled.size() << " cells");
       }
       else
       {
           NS_LOG_WARN("[E2SM-CCC] Unsupported control message format: " << m_controlMessageFormat);
       }
       
       NS_LOG_DEBUG("[E2SM-CCC] Control message decoded successfully");
   }
   catch (const json::exception& e)
   {
       NS_LOG_ERROR("[E2SM-CCC] JSON parsing error in control message: " << e.what());
       NS_LOG_ERROR("[E2SM-CCC] Message content: " << 
                    std::string(reinterpret_cast<char*>(messageBuffer), 
                    std::min(messageSize, size_t(100))));
   }
 }
 
// Helper: return the value of the first matching key found in a JSON object.
// Tries camelCase first (used by the xApp), then snake_case (backward compat).
static const json*
JsonGet(const json& obj, const std::string& camel, const std::string& snake)
{
    if (obj.contains(camel)) return &obj.at(camel);
    if (obj.contains(snake)) return &obj.at(snake);
    return nullptr;
}

void
CccControlMessage::ParseCellControlInfo()
{
  NS_LOG_FUNCTION(this);
  
  try
  {
      // Accepts both camelCase (xApp) and snake_case (legacy) key names.
      const json* cellsListPtr = JsonGet(m_controlMessage,
                                         "listOfCellsControlled",
                                         "list_of_cells_controlled");
      if (!cellsListPtr)
      {
          NS_LOG_ERROR("[E2SM-CCC] Missing 'listOfCellsControlled' in control message");
          return;
      }
      
      const json& cellsList = *cellsListPtr;
      if (!cellsList.is_array())
      {
          NS_LOG_ERROR("[E2SM-CCC] 'listOfCellsControlled' is not an array");
          return;
      }
      
      for (const auto& cellJson : cellsList)
      {
          CellControlInfo cellInfo;
          
          // Cell Global ID
          const json* cgiPtr = JsonGet(cellJson, "cellGlobalId", "cell_global_id");
          if (cgiPtr && cgiPtr->contains("nR-CGI"))
          {
              const json& nrcgi = (*cgiPtr)["nR-CGI"];
              if (nrcgi.contains("pLMNIdentity"))
              {
                  cellInfo.plmn_identity = nrcgi["pLMNIdentity"].get<std::string>();
                  NS_LOG_DEBUG("[E2SM-CCC] PLMN Identity: " << cellInfo.plmn_identity);
              }
              if (nrcgi.contains("nRCellIdentity"))
              {
                  cellInfo.nr_cell_identity = nrcgi["nRCellIdentity"].get<std::string>();
                  NS_LOG_DEBUG("[E2SM-CCC] NR Cell Identity: " << cellInfo.nr_cell_identity);
              }
          }
          
          // Configuration Structures
          const json* configListPtr = JsonGet(cellJson,
                                              "listOfConfigurationStructures",
                                              "list_of_configuration_structures");
          if (configListPtr && configListPtr->is_array())
          {
              for (const auto& configJson : *configListPtr)
              {
                  ConfigurationStructure config;
                  
                  // Structure name
                  const json* namePtr = JsonGet(configJson,
                                                "ranConfigurationStructureName",
                                                "ran_configuration_structure_name");
                  if (namePtr)
                  {
                      config.ran_configuration_structure_name = namePtr->get<std::string>();
                      NS_LOG_INFO("[E2SM-CCC] Configuration Structure: " <<
                                  config.ran_configuration_structure_name);
                  }
                  
                  // Old values
                  const json* oldPtr = JsonGet(configJson,
                                               "oldValuesOfAttributes",
                                               "old_values_of_attributes");
                  if (oldPtr)
                  {
                      config.old_values_of_attributes = *oldPtr;
                      NS_LOG_DEBUG("[E2SM-CCC] Old values: " <<
                                   config.old_values_of_attributes.dump(2));
                  }
                  
                  // New values
                  const json* newPtr = JsonGet(configJson,
                                               "newValuesOfAttributes",
                                               "new_values_of_attributes");
                  if (newPtr)
                  {
                      config.new_values_of_attributes = *newPtr;
                      NS_LOG_DEBUG("[E2SM-CCC] New values: " <<
                                   config.new_values_of_attributes.dump(2));
                      
                      if (config.ran_configuration_structure_name == "O-NESPolicy")
                      {
                          try
                          {
                              config.new_ones_policy =
                                  config.new_values_of_attributes
                                        .get<::quicktype::ONesPolicyProperties>();
                              NS_LOG_INFO("[E2SM-CCC] Parsed new O-NESPolicy");
                          }
                          catch (const std::exception& ex)
                          {
                              NS_LOG_WARN("[E2SM-CCC] Could not parse new O-NESPolicy via quicktype: "
                                          << ex.what() << " (continuing with raw JSON)");
                          }
                      }
                  }
                  
                  // Old O-NESPolicy
                  if (oldPtr && config.ran_configuration_structure_name == "O-NESPolicy")
                  {
                      try
                      {
                          config.old_ones_policy =
                              config.old_values_of_attributes
                                    .get<::quicktype::ONesPolicyProperties>();
                          NS_LOG_INFO("[E2SM-CCC] Parsed old O-NESPolicy");
                      }
                      catch (const std::exception& ex)
                      {
                          NS_LOG_WARN("[E2SM-CCC] Could not parse old O-NESPolicy via quicktype: "
                                      << ex.what() << " (continuing with raw JSON)");
                      }
                  }
                  
                  cellInfo.configuration_structures.push_back(config);
              }
          }
          
          m_cellsControlled.push_back(cellInfo);
      }
      
      NS_LOG_INFO("[E2SM-CCC] Successfully parsed " << m_cellsControlled.size() <<
                  " cells with control information");
  }
  catch (const json::exception& e)
  {
      NS_LOG_ERROR("[E2SM-CCC] Error parsing cell control info: " << e.what());
  }
}
 
 uint32_t
 CccControlMessage::GetRicStyleType() const
 {
   return m_ricStyleType;
 }
 
 CccControlMessage::ControlMessageFormat
 CccControlMessage::GetControlMessageFormat() const
 {
   return m_controlMessageFormat;
 }
 
 std::vector<CellControlInfo>
 CccControlMessage::GetCellsControlled() const
 {
   return m_cellsControlled;
 }
 
 const ConfigurationStructure*
 CccControlMessage::GetConfigurationStructure(
     size_t cellIndex, 
     const std::string& structureName) const
 {
   NS_LOG_FUNCTION(this << cellIndex << structureName);
   
   if (cellIndex >= m_cellsControlled.size())
   {
       NS_LOG_ERROR("[E2SM-CCC] Cell index out of range: " << cellIndex);
       return nullptr;
   }
   
   const CellControlInfo& cellInfo = m_cellsControlled[cellIndex];
   
   for (const auto& config : cellInfo.configuration_structures)
   {
       if (config.ran_configuration_structure_name == structureName)
       {
           return &config;
       }
   }
   
   NS_LOG_WARN("[E2SM-CCC] Configuration structure not found: " << structureName);
   return nullptr;
 }
 
 std::string
 CccControlMessage::GetNewAttributeValue(
     size_t cellIndex,
     const std::string& structureName,
     const std::string& attributeName) const
 {
   NS_LOG_FUNCTION(this << cellIndex << structureName << attributeName);
   
   const ConfigurationStructure* config = GetConfigurationStructure(cellIndex, structureName);
   
   if (config && config->new_values_of_attributes.contains(attributeName))
   {
       return config->new_values_of_attributes[attributeName].dump();
   }
   
   NS_LOG_WARN("[E2SM-CCC] New attribute not found: " << attributeName);
   return "";
 }
 
std::string
CccControlMessage::GetOldAttributeValue(
    size_t cellIndex,
    const std::string& structureName,
    const std::string& attributeName) const
{
  NS_LOG_FUNCTION(this << cellIndex << structureName << attributeName);
  
  const ConfigurationStructure* config = GetConfigurationStructure(cellIndex, structureName);
  
  if (config && config->old_values_of_attributes.contains(attributeName))
  {
      return config->old_values_of_attributes[attributeName].dump();
  }
  
  NS_LOG_WARN("[E2SM-CCC] Old attribute not found: " << attributeName);
  return "";
}

boost::optional<::quicktype::ONesPolicyProperties>
CccControlMessage::GetNewOnesPolicy(size_t cellIndex) const
{
  NS_LOG_FUNCTION(this << cellIndex);
  
  const ConfigurationStructure* config = GetConfigurationStructure(cellIndex, "O-NESPolicy");
  
  if (config && config->new_ones_policy)
  {
      NS_LOG_INFO("[E2SM-CCC] Returning parsed new O-NESPolicy for cell " << cellIndex);
      return config->new_ones_policy;
  }
  
  NS_LOG_WARN("[E2SM-CCC] No new O-NESPolicy found for cell " << cellIndex);
  return boost::optional<::quicktype::ONesPolicyProperties>();
}

boost::optional<::quicktype::ONesPolicyProperties>
CccControlMessage::GetOldOnesPolicy(size_t cellIndex) const
{
  NS_LOG_FUNCTION(this << cellIndex);
  
  const ConfigurationStructure* config = GetConfigurationStructure(cellIndex, "O-NESPolicy");
  
  if (config && config->old_ones_policy)
  {
      NS_LOG_INFO("[E2SM-CCC] Returning parsed old O-NESPolicy for cell " << cellIndex);
      return config->old_ones_policy;
  }
  
  NS_LOG_WARN("[E2SM-CCC] No old O-NESPolicy found for cell " << cellIndex);
  return boost::optional<::quicktype::ONesPolicyProperties>();
}
 
 std::string
 CccControlMessage::GetCellGlobalId(size_t cellIndex) const
 {
   NS_LOG_FUNCTION(this << cellIndex);
   
   if (cellIndex >= m_cellsControlled.size())
   {
       NS_LOG_ERROR("[E2SM-CCC] Cell index out of range: " << cellIndex);
       return "";
   }
   
   const CellControlInfo& cellInfo = m_cellsControlled[cellIndex];
   return cellInfo.plmn_identity + "-" + cellInfo.nr_cell_identity;
 }
 
 void
 CccControlMessage::PrintControlMessage() const
 {
   NS_LOG_FUNCTION(this);
   
   std::cout << "\n========== CCC Control Message ==========\n";
   std::cout << "RAN Function ID: " << m_ranFunctionId << "\n";
   std::cout << "RIC Requestor ID: " << m_ricRequestId.ricRequestorID << "\n";
   std::cout << "RIC Instance ID: " << m_ricRequestId.ricInstanceID << "\n";
   std::cout << "RIC Style Type: " << m_ricStyleType << "\n";
   std::cout << "Control Message Format: " << m_controlMessageFormat << "\n";
   
   std::cout << "\nControl Header (JSON):\n";
   std::cout << m_controlHeader.dump(2) << "\n";   
   
   std::cout << "\nControl Message (JSON):\n";
   std::cout << m_controlMessage.dump(2) << "\n";
   
   std::cout << "\n--- Cells Controlled: " << m_cellsControlled.size() << " ---\n";
   
   for (size_t i = 0; i < m_cellsControlled.size(); ++i)
   {
       const CellControlInfo& cellInfo = m_cellsControlled[i];
       
       std::cout << "\nCell [" << i << "]:\n";
       std::cout << "  PLMN Identity: " << cellInfo.plmn_identity << "\n";
       std::cout << "  NR Cell Identity: " << cellInfo.nr_cell_identity << "\n";
       std::cout << "  Cell Global ID: " << GetCellGlobalId(i) << "\n";
       
       std::cout << "  Configuration Structures: " << cellInfo.configuration_structures.size() << "\n";
       
       for (const auto& config : cellInfo.configuration_structures)
       {
           std::cout << "\n  Configuration: " << config.ran_configuration_structure_name << "\n";
           
           std::cout << "    Old Values:\n";
           for (auto& [key, value] : config.old_values_of_attributes.items())
           {
               std::cout << "      " << key << ": " << value.dump() << "\n";
           }
           
           std::cout << "    New Values:\n";
           for (auto& [key, value] : config.new_values_of_attributes.items())
           {
               std::cout << "      " << key << ": " << value.dump() << "\n";
           }
       }
   }
   
   std::cout << "=========================================\n\n";
 }
 
 } // namespace ns3
 
 