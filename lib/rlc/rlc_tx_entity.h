/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_tx_metrics_container.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/rlc/rlc_tx.h"

namespace srsgnb {

/// Base class used for transmitting RLC bearers.
/// It provides interfaces for the RLC bearers, for both higher layers and lower layers
/// It also stores interfaces for the higher layers, both for the user-plane
/// and the control plane.
class rlc_tx_entity : public rlc_tx_upper_layer_data_interface,
                      public rlc_tx_lower_layer_interface,
                      public rlc_tx_metrics
{
protected:
  rlc_tx_entity(du_ue_index_t                        du_index,
                lcid_t                               lcid,
                rlc_tx_upper_layer_data_notifier&    upper_dn_,
                rlc_tx_upper_layer_control_notifier& upper_cn_,
                rlc_tx_lower_layer_notifier&         lower_dn_) :
    logger("RLC", du_index, lcid), upper_dn(upper_dn_), upper_cn(upper_cn_), lower_dn(lower_dn_)
  {
  }

  bearer_logger                        logger;
  rlc_tx_metrics_container             metrics;
  rlc_tx_upper_layer_data_notifier&    upper_dn;
  rlc_tx_upper_layer_control_notifier& upper_cn;
  rlc_tx_lower_layer_notifier&         lower_dn;

public:
  rlc_tx_metrics get_metrics() { return metrics.get_metrics(); }
  void           reset_metrics() { return metrics.reset_metrics(); }
};

} // namespace srsgnb
