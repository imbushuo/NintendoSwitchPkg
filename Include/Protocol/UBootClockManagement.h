#ifndef __UBOOT_CLOCK_MGMT_H__
#define __UBOOT_CLOCK_MGMT_H__

#include <Uefi.h>
#include <Foundation/Types.h>
#include <Protocol/Utc/Clock.h>

#define TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL_GUID \
    { 0x9c11c451, 0xc497, 0x4e95, { 0xac, 0x18, 0x9f, 0x91, 0xca, 0x8b, 0x9a, 0xd0 } }

typedef struct _TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL;

typedef UINT64 (EFIAPI *clk_get_rate_t)(UINT64 ClkId);
typedef UINT64 (EFIAPI *clk_set_rate_t)(UINT64 ClkId, UINT64 Rate);
typedef void (EFIAPI *clk_enable_t)(UINT64 ClkId);
typedef void (EFIAPI *clk_disable_t)(UINT64 ClkId);
typedef void (EFIAPI *rst_assert_t)(UINT64 RstPId);
typedef void (EFIAPI *rst_deassert_t)(UINT64 RstPId);
typedef void (EFIAPI *rst_periph_t)(UINT64 PeriphId, int Delay);
typedef enum clock_osc_freq (EFIAPI *get_osc_freq_t)(VOID);
typedef unsigned long (EFIAPI *clk_start_pll_t)(enum clock_id clkid, u32 divm, 
    u32 divn, u32 divp, u32 cpcon, u32 lfcon);

struct _TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL {
    clk_get_rate_t GetRate;
    clk_set_rate_t SetRate;
    clk_enable_t EnableClk;
    clk_disable_t DisableClk;
    rst_assert_t AssertRst;
    rst_deassert_t DeassertRst;
    rst_periph_t ResetPeriph;
    get_osc_freq_t GetOscFreq;
    clk_start_pll_t StartPll;
};

extern EFI_GUID gTegraUBootClockManagementProtocolGuid;

#endif