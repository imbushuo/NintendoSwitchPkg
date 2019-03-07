#ifndef __TEGRAMMC_METHODS_H__
#define __TEGRAMMC_METHODS_H__

#include "SdMmc.h"

void tegra_mmc_set_power(
    struct tegra_mmc_priv *priv,
    unsigned short power
);

void tegra_mmc_prepare_data(
    struct tegra_mmc_priv *priv,
    struct mmc_data *data,
    struct bounce_buffer *bbstate
);

void tegra_mmc_set_transfer_mode(
    struct tegra_mmc_priv *priv,
    struct mmc_data *data
);

int tegra_mmc_wait_inhibit(
    struct tegra_mmc_priv *priv,
    struct mmc_cmd *cmd,
    struct mmc_data *data,
    unsigned int timeout
);

int tegra_mmc_send_cmd_bounced(
    struct tegra_mmc_priv *priv, 
    struct mmc_cmd *cmd,
    struct mmc_data *data,
    struct bounce_buffer *bbstate
);

int tegra_mmc_send_cmd(
    struct tegra_mmc_priv *priv, 
    struct mmc_cmd *cmd,
    struct mmc_data *data
);

void tegra_mmc_pad_init(
	struct tegra_mmc_priv *priv
);

void tegra_mmc_change_clock(
	struct tegra_mmc_priv *priv, 
	uint clock
);

int tegra_mmc_set_ios(
    struct mmc* mMmcInstance
);

#endif