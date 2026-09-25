
#ifndef LENA_INDICATION_MESSAGE_HELPER_H
#define LENA_INDICATION_MESSAGE_HELPER_H

#include <ns3/indication-message-helper.h>
#include <array>
namespace ns3 {
class LenaIndicationMessageHelper : public IndicationMessageHelper
{
public:
   LenaIndicationMessageHelper (IndicationMessageType type, bool isOffline, bool reducedPmValues);
   ~LenaIndicationMessageHelper ();

  void AddPositioningFeatures(
    const std::string& ueImsiComplete,
    const std::array<double, 32>& features);

  void AddPdcpUePmItem (std::string ueImsiComplete, long txPdcpPduBytesNrRlc, long txPdcpPduNrRlc, double pdcpThroughput);
  void AddPdcpCpUePmItem ();
  void AddRlcUePmItem ();
  void AddMacUePmItem ();
  void AddPhyUePmItem ();
  void AddPHYGnbConfiguration (uint16_t numActiveUes, uint16_t cellId, uint16_t portsOn, uint16_t portsOff);

  /**
   * \brief Add a single PHY-layer KPI measurement to the indication message.
   *
   * Each call appends one measurement item to the UE (or cell) indication list,
   * keyed by the IMSI string.  Call once per sample produced by NrPhyKpiCollector.
   *
   * \param kpiName   The KPI name (e.g. "PHY.DlDataSinr").
   * \param value     Numeric value (doubles, uint64 and bool are all mapped to double).
   * \param imsi      UE IMSI; pass 0 for cell-level or global KPIs.
   * \param cellId    Serving cell identifier.
   */
  void AddPhyKpiItem (const std::string& kpiName, double value,
                      uint64_t imsi, uint16_t cellId);
  void AddPdcpCpGnbPmItem ();
  void AddRlcGnbPmItem ();
  void AddMacGnbPmItem ();
  void AddPhyGnbPmItem ();
};
} // namespace ns3
#endif /* LENA_INDICATION_MESSAGE_HELPER_H */
 
