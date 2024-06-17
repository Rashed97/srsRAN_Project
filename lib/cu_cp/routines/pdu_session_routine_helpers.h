/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/ue_configuration.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/rrc/rrc_types.h"

namespace srsran {
namespace srs_cu_cp {

bool verify_and_log_cell_group_config(const byte_buffer& cell_group_cfg, const srslog::basic_logger& logger);

void fill_e1ap_drb_pdcp_config(e1ap_pdcp_config& e1ap_pdcp_cfg, const pdcp_config& cu_cp_pdcp_cfg);
void fill_e1ap_qos_flow_param_item(e1ap_qos_flow_qos_param_item&      e1ap_qos_item,
                                   const srslog::basic_logger&        logger,
                                   const qos_flow_setup_request_item& request_item);
void fill_e1ap_bearer_context_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& e1ap_list,
    const slotted_id_vector<drb_id_t, f1ap_drbs_setup_mod_item>&              drb_setup_items,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>&          pdu_sessions_update_list);
void fill_e1ap_pdu_session_res_to_setup_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item>&     pdu_session_res_to_setup_list,
    const srslog::basic_logger&                                                  logger,
    const up_config_update&                                                      next_config,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
    const ue_configuration&                                                      ue_cfg,
    const security_indication_t&                                                 default_security_indication);
void fill_drb_to_setup_list(slotted_id_vector<drb_id_t, e1ap_drb_to_setup_item_ng_ran>&          e1ap_drb_to_setup_list,
                            const slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item>& qos_flow_list,
                            const std::map<drb_id_t, up_drb_context>&                            drb_to_add_list,
                            const srslog::basic_logger&                                          logger);
void fill_drb_to_remove_list(std::vector<drb_id_t>&       e1ap_drb_to_remove_list,
                             const std::vector<drb_id_t>& drb_to_remove_list);

/// \brief Fill RRC Reconfiguration message content.
/// \param[out] rrc_reconfig_args The RRC Reconfiguration Arguments struct to fill.
/// \param[in] srbs_to_be_setup_mod_list SRBs to be setup (only needed if default DRB is being setup).
/// \param[in] pdu_sessions The PDU sessions to add/mod in the reconfiguration.
/// \param[in] drb_to_remove DRB to remove from the configurations.
/// \param[in] du_to_cu_rrc_info RRC container forwarded from the DU to the CU.
/// \param[in] nas_pdus NAS PDUs to forward to the UE as received by the AMF.
/// \param[in] rrc_meas_cfg Optional measurement config to include in Reconfiguration.
/// \param[in] reestablish_srbs Whether to request SRB reestablishment.
/// \param[in] reestablish_drbs Whether to request DRB reestablishment.
/// \param[in] sib1 Packed SIB1 content for direct delivery to UE (optional).
/// \param[in] logger Logger reference.
/// \return True on success, false otherwise.
bool fill_rrc_reconfig_args(
    rrc_reconfiguration_procedure_request&                             rrc_reconfig_args,
    const slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item>& srbs_to_be_setup_mod_list,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>&   pdu_sessions,
    const std::vector<drb_id_t>&                                       drb_to_remove,
    const f1ap_du_to_cu_rrc_info&                                      du_to_cu_rrc_info,
    const std::vector<byte_buffer>&                                    nas_pdus,
    const std::optional<rrc_meas_cfg>                                  rrc_meas_cfg,
    bool                                                               reestablish_srbs,
    bool                                                               reestablish_drbs,
    bool                                                               update_keys,
    byte_buffer                                                        sib1,
    const srslog::basic_logger&                                        logger);

bool update_setup_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>& ngap_response_list,
    slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item>&                    srb_setup_mod_list,
    slotted_id_vector<drb_id_t, f1ap_drbs_to_be_setup_mod_item>&                    drb_setup_mod_list,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>&    ngap_setup_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                 pdu_session_resource_setup_list,
    up_config_update&            next_config,
    up_resource_manager&         rrc_ue_up_resource_manager,
    const security_indication_t& default_security_indication,
    const srslog::basic_logger&  logger);

bool update_setup_list(slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item>&                 srb_setup_mod_list,
                       slotted_id_vector<drb_id_t, f1ap_drbs_to_be_setup_mod_item>&                 drb_setup_mod_list,
                       const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& ngap_setup_list,
                       const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                                   pdu_session_resource_setup_list,
                       up_config_update&           next_config,
                       up_resource_manager&        rrc_ue_up_resource_manager,
                       const srslog::basic_logger& logger);

bool update_setup_list(e1ap_bearer_context_modification_request&                    bearer_ctxt_mod_request,
                       const slotted_id_vector<drb_id_t, f1ap_drbs_setup_mod_item>& drb_setup_mod_list,
                       const up_config_update&                                      next_config,
                       const srslog::basic_logger&                                  logger);

void update_failed_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>&     ngap_failed_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>& pdu_session_resource_failed_list);

/// \brief Processes the result of a Bearer Context Modifcation Result's PDU session modify list.
/// \param[out] ngap_response_list Reference to the final NGAP response
/// \param[out] ue_context_mod_request Reference to the next request message - a UE context modification.
/// \param[in] ngap_modify_list Const reference to the original NGAP request
/// \param[in] e1ap_pdu_session_resource_modify_list Const reference to the response of the previous subprocedure
/// \param[in]next_config Const reference to the calculated config update
/// \param[in] logger Reference to the logger.
/// \return True on success, false otherwise.
bool update_modify_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
    f1ap_ue_context_modification_request&                                                 ue_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>&
                                e1ap_pdu_session_resource_modify_list,
    up_config_update&           next_config,
    const srslog::basic_logger& logger);

/// \brief Processes the response of a UE Context Modifcation Request.
/// \param[out] ngap_response_list Reference to the final NGAP response.
/// \param[out] ue_context_mod_request Reference to the next request message - a Bearer context modification request.
/// \param[in] ngap_modify_list Const reference to the original NGAP request
/// \param[in] ue_context_modification_response Const reference to the response of the UE context modifcation request.
/// \param[in] next_config Const reference to the calculated config update
/// \param[in] logger Reference to the logger.
/// \return True on success, false otherwise.
bool update_modify_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
    e1ap_bearer_context_modification_request&                                             bearer_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const f1ap_ue_context_modification_response& ue_context_modification_response,
    const up_config_update&                      next_config,
    const srslog::basic_logger&                  logger);

} // namespace srs_cu_cp
} // namespace srsran
