/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_security_manager_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_security_manager::ue_security_manager(const security_manager_config& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

// up_ue_security_manager
security::sec_as_config ue_security_manager::get_up_as_config() const
{
  return sec_context.get_as_config(security::sec_domain::up);
}

security::sec_128_as_config ue_security_manager::get_up_128_as_config() const
{
  return sec_context.get_128_as_config(security::sec_domain::up);
}

// ngap_ue_security_manager
bool ue_security_manager::init_security_context(security::security_context sec_ctxt)
{
  // Update the security context
  sec_context = sec_ctxt;

  // Select security algorithms
  logger.debug("Integrity protection algorithms preference list: {}", cfg.int_algo_pref_list);
  logger.debug("Ciphering algorithms preference list: {}", cfg.enc_algo_pref_list);
  logger.debug("Integrity protection algorithms supported list: {}", sec_context.supported_int_algos);
  logger.debug("Ciphering algorithms preference list: {}", sec_context.supported_enc_algos);
  if (not sec_context.select_algorithms(cfg.int_algo_pref_list, cfg.enc_algo_pref_list)) {
    logger.error("Could not select security algorithm");
    return false;
  }
  logger.debug("Selected security algorithms integrity=NIA{} ciphering=NEA{}",
               sec_context.sel_algos.integ_algo,
               sec_context.sel_algos.cipher_algo);

  // Generate K_rrc_enc and K_rrc_int
  sec_context.generate_as_keys();

  return true;
}

bool ue_security_manager::is_security_enabled() const
{
  return security_enabled;
}

// rrc_ue_security_manager

void ue_security_manager::enable_security()
{
  security_enabled = true;
}

security::security_context ue_security_manager::get_security_context() const
{
  return sec_context;
}

security::sec_selected_algos ue_security_manager::get_security_algos() const
{
  return sec_context.sel_algos;
}

security::sec_as_config ue_security_manager::get_rrc_as_config() const
{
  return sec_context.get_as_config(security::sec_domain::rrc);
}

security::sec_128_as_config ue_security_manager::get_rrc_128_as_config() const
{
  return sec_context.get_128_as_config(security::sec_domain::rrc);
}

void ue_security_manager::update_security_context(security::security_context sec_ctxt)
{
  sec_context = sec_ctxt;
}

void ue_security_manager::horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn)
{
  sec_context.horizontal_key_derivation(target_pci, target_ssb_arfcn);
}