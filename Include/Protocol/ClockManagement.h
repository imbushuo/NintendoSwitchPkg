#ifndef __CLOCK_MANAGEMENT_PROTOCOL_H__
#define __CLOCK_MANAGEMENT_PROTOCOL_H__

#define TEGRA210_CLOCK_MANAGEMENT_PROTOCOL_GUID \
    { 0x9c11c45d, 0xc497, 0x4e95, { 0xac, 0x18, 0x9f, 0x91, 0xca, 0x8b, 0x9a, 0xd0 } }

typedef struct _TEGRA210_CLOCK_MGMT_PROTOCOL TEGRA210_CLOCK_MGMT_PROTOCOL;

typedef EFI_STATUS (EFIAPI *clk_uart_enable_t)(UINT32 DeviceIndex);
typedef EFI_STATUS (EFIAPI *clk_i2c_enable_t)(UINT32 DeviceIndex);
typedef EFI_STATUS (EFIAPI *clk_cl_dvfs_enable_t)(VOID);
typedef BOOLEAN    (EFIAPI *clk_sdmmc_query_clk_not_reset_and_enabled_t)(UINT32 Index);
typedef EFI_STATUS (EFIAPI *clk_sdmmc_get_params_t)(UINT32 *pOut, UINT16 *pDivisor, UINT32 Type);
typedef EFI_STATUS (EFIAPI *clk_sdmmc_enable_t)(UINT32 Index, UINT32 Clock);
typedef EFI_STATUS (EFIAPI *clk_sdmmc_disable_t)(UINT32 Index);
typedef EFI_STATUS (EFIAPI *clk_sdmmc_sel_clk_src_t)(UINT32 *pOut, UINT32 Index, UINT32 Value);


struct _TEGRA210_CLOCK_MGMT_PROTOCOL {
    clk_uart_enable_t       EnableUart;
    clk_i2c_enable_t        EnableI2c;
    clk_cl_dvfs_enable_t    EnableClDvfs;
    clk_sdmmc_query_clk_not_reset_and_enabled_t QuerySdMmcClkNotResetAndEnabled;
    clk_sdmmc_get_params_t  QuerySdMmcClkParams;
    clk_sdmmc_enable_t      EnableSdMmcClk;
    clk_sdmmc_sel_clk_src_t SelectSdMmcClkSource;
    clk_sdmmc_disable_t     DisableSdMmcClk;
};

extern EFI_GUID gTegra210ClockManagementProtocolGuid;

#endif
