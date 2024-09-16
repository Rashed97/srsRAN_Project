/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_ue_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_ue::cu_cp_ue(ue_index_t                     ue_index_,
                   du_index_t                     du_index_,
                   const up_resource_manager_cfg& up_cfg,
                   const security_manager_config& sec_cfg,
                   ue_task_scheduler_impl         task_sched_,
                   plmn_identity                  plmn_,
                   std::optional<gnb_du_id_t>     du_id_,
                   std::optional<pci_t>           pci_,
                   std::optional<rnti_t>          c_rnti_,
                   std::optional<du_cell_index_t> pcell_index_) :
  ue_index(ue_index_),
  task_sched(std::move(task_sched_)),
  up_mng(up_cfg),
  sec_mng(sec_cfg),
  rrc_ue_cu_cp_ev_notifier(ue_index)
{
  if (du_id_.has_value() && du_id_.value() != gnb_du_id_t::invalid) {
    ue_ctxt.du_id = du_id_.value();
  }

  if (pci_.has_value() && pci_.value() != INVALID_PCI) {
    pci = pci_.value();
  }

  if (c_rnti_.has_value() && c_rnti_.value() != rnti_t::INVALID_RNTI) {
    ue_ctxt.crnti = c_rnti_.value();
  }

  if (pcell_index_.has_value() && pcell_index_.value() != du_cell_index_t::invalid) {
    pcell_index = pcell_index_.value();
  }

  ue_ctxt.du_idx = du_index_;
  ue_ctxt.plmn   = plmn_;

  rrc_ue_cu_cp_ue_ev_notifier.connect_ue(*this);
  ngap_cu_cp_ue_ev_notifier.connect_ue(*this);
}

/// \brief Update a UE with PCI and/or C-RNTI.
void cu_cp_ue::update_du_ue(gnb_du_id_t du_id_, pci_t pci_, rnti_t c_rnti_, du_cell_index_t pcell_index_)
{
  if (du_id_ != gnb_du_id_t::invalid) {
    ue_ctxt.du_id = du_id_;
  }

  if (pci_ != INVALID_PCI) {
    pci = pci_;
  }

  if (c_rnti_ != rnti_t::INVALID_RNTI) {
    ue_ctxt.crnti = c_rnti_;
  }

  if (pcell_index_ != du_cell_index_t::invalid) {
    pcell_index = pcell_index_;
  }
}

/// \brief Set/update the measurement context of the UE.
void cu_cp_ue::update_meas_context(cell_meas_manager_ue_context meas_ctxt)
{
  meas_context = std::move(meas_ctxt);
}

/// \brief Set the RRC UE of the UE.
/// \param[in] rrc_ue_ RRC UE of the UE.
void cu_cp_ue::set_rrc_ue(rrc_ue_interface& rrc_ue_)
{
  rrc_ue = &rrc_ue_;
}

/// \brief Get the NGAP RRC UE notifier of the UE.
ngap_rrc_ue_notifier& cu_cp_ue::get_ngap_rrc_ue_notifier()
{
  return ngap_rrc_ue_ev_notifier;
}
