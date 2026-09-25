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

 #ifndef CCC_MESSAGE_H
 #define CCC_MESSAGE_H
 
 #include <assert.h>
 #include "ns3/object.h"
 #include <ns3/asn1c-types.h>
 #include <vector>
 
 #define JSON_DIAGNOSTICS 1
 #define JSON_NO_IO 1
 #include <nlohmann/json.hpp>
 #include <e2sim/e2sm_ccc.hpp>
 
 extern "C" {
   #include "E2AP-PDU.h"
   #include "RICcontrolRequest.h"
   #include "ProtocolIE-Field.h"
   #include "InitiatingMessage.h"
 }
 
 namespace ns3 {
 
   using json = nlohmann::json;

  /**
   * \brief Represents a RAN configuration structure for O-NESPolicy

   */
  struct ConfigurationStructure
  {
      std::string ran_configuration_structure_name;
      json old_values_of_attributes; 
      json new_values_of_attributes;  
      
      // Parsed O-NESPolicy objects
      boost::optional<::quicktype::ONesPolicyProperties> old_ones_policy;
      boost::optional<::quicktype::ONesPolicyProperties> new_ones_policy;
  };

  /**
   * \brief Represents cell control information
   * Uses proper classes from e2sm_ccc.hpp 
   */
  struct CellControlInfo
  {
      std::string plmn_identity;
      std::string nr_cell_identity;
      std::vector<ConfigurationStructure> configuration_structures;
  };
 
   /**
    * \brief CCC Control Message Handler
    * 
    * This class handles RIC Control Messages for the E2SM-CCC service model
    * (Cell Configuration and Control). Unlike ASN.1-based service models,
    * CCC uses JSON encoding for control messages.
    */
   class CccControlMessage : public SimpleRefCount<CccControlMessage>
   {
   public:
     /**
      * \brief CCC CONTROL Service Style Types
      * 
      * As defined in O-RAN.WG3.TS.E2SM-CCC specification:
      * - Node Configuration and Control: Control node configurations
      * - Cell Configuration and Control: Control cell configurations
      */
     enum ControlStyleType {
       Node_Configuration_and_Control = 1,
       Cell_Configuration_and_Control = 2
     };
     

     enum ControlHeaderFormat {
        FORMAT_1_CCC_CTRL_HDR = 1
      };
  
      /**
       * \brief Control Message Format Types
       */
      enum ControlMessageFormat {
        FORMAT_1_CCC_CTRL_MSG = 1,
        FORMAT_2_CCC_CTRL_MSG = 2
      };
     /**
      * \brief Constructor
      * \param pdu E2AP PDU containing the RIC Control Request
      */
     CccControlMessage (E2AP_PDU_t *pdu);
     
     /**
      * \brief Destructor
      */
     ~CccControlMessage ();
 
    /**
     * \brief Get the RIC style type
     * \return The RIC style type
     */
    uint32_t GetRicStyleType() const;

    /**
     * \brief Get the control message format
     * \return The control message format
     */
    ControlMessageFormat GetControlMessageFormat() const;

    /**
     * \brief Get the list of cells controlled
     * \return Vector of CellControlInfo
     */
    std::vector<CellControlInfo> GetCellsControlled() const;

    /**
     * \brief Get the cell global ID for a specific cell
     * \param cellIndex Index of the cell in the controlled cells list
     * \return The cell global ID as string
     */
    std::string GetCellGlobalId(size_t cellIndex) const;
 
     /**
      * \brief Get the control style type from the control message
      * \return The control style type
      */
     ControlStyleType GetControlStyleType() const;
 
     /**
      * \brief Get the target cell ID for the control action
      * \return The target cell ID as string
      */
     std::string GetTargetCellId() const;
 
     /**
      * \brief Get control parameters as JSON object
      * \return JSON object containing control parameters
      */
     json GetControlParameters() const;
 
     /**
      * \brief Get a specific control parameter value
      * \param parameterName Name of the parameter to retrieve
      * \return The parameter value as string, or empty string if not found
      */
     std::string GetControlParameter(const std::string& parameterName) const;

    /**
     * \brief Get a configuration structure for a specific cell
     * \param cellIndex Index of the cell
     * \param structureName Name of the configuration structure
     * \return Pointer to the configuration structure, or nullptr if not found
     */
    const ConfigurationStructure* GetConfigurationStructure(
        size_t cellIndex, const std::string& structureName) const;

    /**
     * \brief Get a new attribute value from a configuration structure
     * \param cellIndex Index of the cell
     * \param structureName Name of the configuration structure
     * \param attributeName Name of the attribute
     * \return The attribute value as string, or empty string if not found
     */
    std::string GetNewAttributeValue(
        size_t cellIndex,
        const std::string& structureName,
        const std::string& attributeName) const;

    /**
     * \brief Get an old attribute value from a configuration structure
     * \param cellIndex Index of the cell
     * \param structureName Name of the configuration structure
     * \param attributeName Name of the attribute
     * \return The attribute value as string, or empty string if not found
     */
    std::string GetOldAttributeValue(
        size_t cellIndex,
        const std::string& structureName,
        const std::string& attributeName) const;

    /**
     * \brief Get the parsed O-NESPolicy (new values) for a specific cell
     * \param cellIndex Index of the cell
     * \return Optional ONesPolicyProperties object
     */
    boost::optional<::quicktype::ONesPolicyProperties> GetNewOnesPolicy(size_t cellIndex) const;

    /**
     * \brief Get the parsed O-NESPolicy (old values) for a specific cell
     * \param cellIndex Index of the cell
     * \return Optional ONesPolicyProperties object
     */
    boost::optional<::quicktype::ONesPolicyProperties> GetOldOnesPolicy(size_t cellIndex) const;
 
     /**
      * \brief Check if a specific parameter exists in control message
      * \param parameterName Name of the parameter to check
      * \return true if parameter exists, false otherwise
      */
     bool HasControlParameter(const std::string& parameterName) const;
 
     /**
      * \brief Get the RIC Request ID
      * \return RIC Request ID structure
      */
     RICrequestID_t GetRicRequestId() const { return m_ricRequestId; }
 
     /**
      * \brief Get the RAN Function ID
      * \return RAN Function ID
      */
     RANfunctionID_t GetRanFunctionId() const { return m_ranFunctionId; }
 
     /**
      * \brief Print the control message contents for debugging
      */
     void PrintControlMessage() const;
 
   private:
     /**
      * \brief Decodes the RIC Control message from E2AP PDU
      * \param pdu PDU passed by the RIC
      */
     void DecodeRicControlMessage (E2AP_PDU_t *pdu);
 
     /**
      * \brief Decodes the CCC control header (JSON format)
      * \param headerBuffer Buffer containing the control header
      * \param headerSize Size of the header buffer
      */
     void DecodeCccControlHeader(uint8_t* headerBuffer, size_t headerSize);
 
     /**
      * \brief Decodes the CCC control message (JSON format)
      * \param messageBuffer Buffer containing the control message
      * \param messageSize Size of the message buffer
      */
     void DecodeCccControlMessage(uint8_t* messageBuffer, size_t messageSize);

    /**
     * \brief Parse cell control information from the control message
     */
    void ParseCellControlInfo();
 
     // Member variables
    uint32_t m_ricStyleType;
    ControlHeaderFormat m_controlHeaderFormat;
    ControlMessageFormat m_controlMessageFormat;
     ControlStyleType m_controlStyleType;
     long m_controlActionId;
     std::string m_targetCellId;
     json m_controlHeader;
     json m_controlMessage;
     json m_controlParameters;
    std::vector<CellControlInfo> m_cellsControlled;
     
     RANfunctionID_t m_ranFunctionId;
     RICrequestID_t m_ricRequestId;
     RICcallProcessID_t m_ricCallProcessId;
   };
 
 } // namespace ns3
 
 #endif /* CCC_MESSAGE_H */
 
 