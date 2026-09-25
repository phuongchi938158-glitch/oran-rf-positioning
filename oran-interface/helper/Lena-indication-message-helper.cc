/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Northeastern University
 * Copyright (c) 2022 Sapienza, University of Rome
 * Copyright (c) 2022 University of Padova
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


#include "Lena-indication-message-helper.h"
#include <cstdio>
namespace ns3 {
LenaIndicationMessageHelper::LenaIndicationMessageHelper (IndicationMessageType type,
                                                                 bool isOffline, bool reducedPmValues)
    : IndicationMessageHelper (type, isOffline, reducedPmValues)
{
}   

void
LenaIndicationMessageHelper::AddPositioningFeatures(
    const std::string& ueImsiComplete,
    const std::array<double, 32>& features)
{
    // ONE MeasurementItemList for ONE UE
    Ptr<MeasurementItemList> ueVal =
        Create<MeasurementItemList>(ueImsiComplete);

    if (!m_reducedPmValues)
    {
        for (size_t i = 0; i < 32; ++i)
        {
            char name[16];

            std::snprintf(
                name,
                sizeof(name),
                "POS.F%02zu",
                i);

            ueVal->AddItem<double>(
                name,
                features[i]);
        }
    }

    // Insert ONCE, after all 32 features are inside
    m_msgValues.m_ueIndications.insert(ueVal);
}

void
LenaIndicationMessageHelper::AddPdcpUePmItem (std::string ueImsiComplete,
                                                long txPdcpPduBytesNrRlc, long txPdcpPduNrRlc, double pdcpThroughput)
{
  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    { 
       std::cout << "[AddPdcpUePmItem] IMSI=" << ueImsiComplete << std::endl;
       std::cout << "  Adding QosFlow.PdcpPduVolumeDL_Filter.UEID = " << txPdcpPduBytesNrRlc << std::endl;
      ueVal->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", txPdcpPduBytesNrRlc);
      
      std::cout << "  Adding DLThroughput.UEID = " << pdcpThroughput << std::endl;
      ueVal->AddItem<double> ("DLThroughput.UEID", pdcpThroughput);

        std::cout << "  Adding DRB.PdcpPduNbrDl.Qos.UEID = " << txPdcpPduNrRlc << std::endl;
      ueVal->AddItem<long> ("DRB.PdcpPduNbrDl.Qos.UEID", txPdcpPduNrRlc);
    }

  m_msgValues.m_ueIndications.insert (ueVal);
}
void
LenaIndicationMessageHelper::AddPdcpCpUePmItem (){
  std::cout << "[DEBUG] AddPdcpCpUePmItem called" << std::endl;
}

void
LenaIndicationMessageHelper::AddRlcUePmItem (){
  std::cout << "[DEBUG] AddRlcUePmItem called" << std::endl;
}

void
LenaIndicationMessageHelper::AddMacUePmItem (){
  std::cout << "[DEBUG] AddMacUePmItem called" << std::endl;
}

void
LenaIndicationMessageHelper::AddPhyUePmItem (){
  std::cout << "[DEBUG] AddPhyUePmItem called" << std::endl;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


void
LenaIndicationMessageHelper::AddPHYGnbConfiguration (uint16_t numActiveUes, uint16_t cellId, uint16_t portsOn, uint16_t portsOff){
  std::cout << "[DEBUG] AddPHYGnbConfiguration called" << std::endl;
    // Create a cell-level measurement item list (empty IMSI string for cell-level)
    Ptr<MeasurementItemList> cellVal = Create<MeasurementItemList> ("");
  
    if (!m_reducedPmValues)
    {
      // Add RRC connections (number of active UEs) 
      cellVal->AddItem<long> ("CellId", cellId);
      cellVal->AddItem<long> ("RRCEstabConn", numActiveUes);
      cellVal->AddItem<long> ("Old.PortsOn", portsOn);
      cellVal->AddItem<long> ("Old.PortsOff", portsOff);
    }
    
    m_msgValues.m_ueIndications.insert (cellVal);
}

void
LenaIndicationMessageHelper::AddPdcpCpGnbPmItem (){
  std::cout << "[DEBUG] AddPdcpCpGnbPmItem called" << std::endl;
}

void
LenaIndicationMessageHelper::AddRlcGnbPmItem (){
  std::cout << "[DEBUG] AddRlcGnbPmItem called" << std::endl;
}

void
LenaIndicationMessageHelper::AddMacGnbPmItem (){
  std::cout << "[DEBUG] AddMacGnbPmItem called" << std::endl;
}

void
LenaIndicationMessageHelper::AddPhyGnbPmItem (){
  std::cout << "[DEBUG] AddPhyGnbPmItem called" << std::endl;
}


void
LenaIndicationMessageHelper::AddPhyKpiItem (const std::string& kpiName,
                                             double value,
                                             uint64_t imsi,
                                             uint16_t cellId)
{
  // Build a key that matches what GetImsiString() produces in NrGnbNetDevice:
  // cell-level/global KPIs (imsi == 0) use an empty key string.
  std::string key = (imsi == 0) ? "" : std::to_string (imsi);

  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (key);
  if (!m_reducedPmValues)
    {
      std::cout << "[AddPhyKpiItem] imsi=" << imsi
                << " cell=" << cellId
                << " kpi=" << kpiName
                << " val=" << value << std::endl;
      ueVal->AddItem<double> (kpiName, value);
    }

  m_msgValues.m_ueIndications.insert (ueVal);
}

LenaIndicationMessageHelper::~LenaIndicationMessageHelper ()
{
}
}
