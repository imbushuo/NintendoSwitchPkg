#ifndef __UBOOT_CLOCK_MGMT_H__
#define __UBOOT_CLOCK_MGMT_H__

#define TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL_GUID \
    { 0x9c11c451, 0xc497, 0x4e95, { 0xac, 0x18, 0x9f, 0x91, 0xca, 0x8b, 0x9a, 0xd0 } }

typedef struct _TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL;

typedef UINT64 (EFIAPI *clk_get_rate_t)(UINT64 ClkId);
typedef UINT64 (EFIAPI *clk_set_rate_t)(UINT64 ClkId, UINT64 Rate);
typedef void (EFIAPI *clk_enable_t)(UINT64 ClkId);
typedef void (EFIAPI *clk_disable_t)(UINT64 ClkId);
typedef void (EFIAPI *rst_assert_t)(UINT64 RstPId);
typedef void (EFIAPI *rst_deassert_t)(UINT64 RstPId);

struct _TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL {
    clk_get_rate_t GetRate;
    clk_set_rate_t SetRate;
    clk_enable_t EnableClk;
    clk_disable_t DisableClk;
    rst_assert_t AssertRst;
    rst_deassert_t DeassertRst;
};

extern EFI_GUID gTegraUBootClockManagementProtocolGuid;

#endif