/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_resource_release_routine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

pdu_session_resource_release_routine::pdu_session_resource_release_routine(
    const cu_cp_pdu_session_resource_release_command& release_cmd_,
    du_processor_e1ap_control_notifier&               e1ap_ctrl_notif_,
    du_processor_f1ap_ue_context_notifier&            f1ap_ue_ctxt_notif_,
    du_processor_ngap_control_notifier&               ngap_ctrl_notifier_,
    du_processor_ue_task_scheduler&                   task_sched_,
    up_resource_manager&                              rrc_ue_up_resource_manager_,
    srslog::basic_logger&                             logger_) :
  release_cmd(release_cmd_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  ngap_ctrl_notifier(ngap_ctrl_notifier_),
  task_sched(task_sched_),
  rrc_ue_up_resource_manager(rrc_ue_up_resource_manager_),
  logger(logger_)
{
}

void pdu_session_resource_release_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_release_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized", release_cmd.ue_index, name());

  // Perform initial sanity checks on incoming message.
  if (!rrc_ue_up_resource_manager.validate_request(release_cmd)) {
    logger.warning("ue={}: \"{}\" Invalid PduSessionResourceReleaseCommand", release_cmd.ue_index, name());
    CORO_EARLY_RETURN(generate_pdu_session_resource_release_response(false));
  }

  {
    // Calculate next user-plane configuration based on incoming release command.
    next_config = rrc_ue_up_resource_manager.calculate_update(release_cmd);
  }

  // Inform CU-UP about the release of a bearer
  // Note: The Bearers must be released at CU-UP before the DRBs are released at DU.
  if (next_config.context_removal_required) {
    // Remove bearer context.
    bearer_context_release_command.ue_index = release_cmd.ue_index;
    bearer_context_release_command.cause    = cause_radio_network_t::unspecified;

    CORO_AWAIT(e1ap_ctrl_notifier.on_bearer_context_release_command(bearer_context_release_command));

    // Request UE context removal.
    logger.info("ue={}: \"{}\" Requesting UE context release", release_cmd.ue_index, name());
    ue_context_release_request.ue_index = release_cmd.ue_index;
    ue_context_release_request.cause    = cause_radio_network_t::unknown_pdu_session_id;
    CORO_AWAIT(ngap_ctrl_notifier.on_ue_context_release_request(ue_context_release_request));
  } else {
    // prepare BearerContextModificationRequest and call e1 notifier
    bearer_context_modification_request.ue_index = release_cmd.ue_index;

    for (const auto& pdu_session_res_to_release : next_config.pdu_sessions_to_remove_list) {
      e1ap_ng_ran_bearer_context_mod_request bearer_context_mod_request;
      bearer_context_mod_request.pdu_session_res_to_rem_list.push_back(pdu_session_res_to_release);
      bearer_context_modification_request.ng_ran_bearer_context_mod_request = bearer_context_mod_request;
    }

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (not bearer_context_modification_response.success) {
      logger.warning("ue={}: \"{}\" failed to release bearer(s) at CU-UP", release_cmd.ue_index, name());
    }
  }

  // Release DRB resources at DU
  {
    // prepare UeContextModificationRequest and call F1 notifier
    ue_context_mod_request.ue_index = release_cmd.ue_index;
    for (const auto& drb_id : next_config.drb_to_remove_list) {
      ue_context_mod_request.drbs_to_be_released_list.push_back(drb_id);
    }

    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (not ue_context_modification_response.success) {
      logger.warning("ue={}: \"{}\" failed to release bearer(s) at DU", release_cmd.ue_index, name());
    }
  }

  CORO_RETURN(generate_pdu_session_resource_release_response(true));
}

cu_cp_pdu_session_resource_release_response
pdu_session_resource_release_routine::generate_pdu_session_resource_release_response(bool success)
{
  if (success) {
    for (const auto& setup_item : release_cmd.pdu_session_res_to_release_list_rel_cmd) {
      cu_cp_pdu_session_res_released_item_rel_res item;
      item.pdu_session_id = setup_item.pdu_session_id;

      // TODO: Add pdu_session_res_release_resp_transfer

      response_msg.pdu_session_res_released_list_rel_res.emplace(setup_item.pdu_session_id, item);
    }

    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_remove : next_config.pdu_sessions_to_remove_list) {
      result.pdu_sessions_removed_list.push_back(pdu_session_to_remove);
    }

    rrc_ue_up_resource_manager.apply_config_update(result);
  }

  return response_msg;
}
