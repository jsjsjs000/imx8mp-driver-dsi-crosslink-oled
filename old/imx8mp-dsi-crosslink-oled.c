// SPDX-License-Identifier: GPL-2.0
/*
 * based on: drivers/gpu/drm/bridge/tc358867xbg.c
 * and: drivers/gpu/drm/panel/panel-rpi-7inch.c
 * and: drivers/gpu/drm/bridge/tc358767.c
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/media-bus-format.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>
#include <drm/drm_print.h>
#include <drm/rpi_display.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>
#include <video/of_videomode.h>

// Basic Registers
#define POCTRL			0x0448
#define DP0_SRCCTRL		0x06a0
#define PXL_PLLPARAM		0x0914
#define PXL_PLLCTRL		0x0908
#define SYS_PLLPARAM		0x0918
#define DP_PHY_CTRL		0x0800
#define PPI_TX_RX_TA		0x013c
#define LPTXTIMCNT		0x0114
#define D0S_CLRSIPOCOUNT	0x0164
#define D1S_CLRSIPOCOUNT	0x0168
#define D2S_CLRSIPOCOUNT	0x016c
#define D3S_CLRSIPOCOUNT	0x0170
#define PPI_LANEENABLE		0x0134
#define DSI_LANEENABLE		0x0210
#define STARTPPI		0x0104
#define STARTDSI		0x0204

// Timing Registers
#define VPCTRL			0x0450
#define HTIM01			0x0454
#define VTIM01			0x045c
#define VFUEN0			0x0464
#define SYSCTRL			0x0510

// Panel for DRM
struct panel_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;
	const struct display_timing *timings;
	unsigned int num_timings;

	unsigned int bpc;

	struct {
		unsigned int width;
		unsigned int height;
	} size;

	u32 bus_format;
};

struct dsi_crosslink {
	struct drm_panel base;
	bool enabled;

	struct device *dev;
	struct mipi_dsi_device *dsi;
	const struct panel_desc *desc;
};

static const struct drm_display_mode dsi_crosslink_default_mode = {
	.clock = 25175,
	.hdisplay = 800,
	.hsync_start = 800 + 40,
	.hsync_end = 800 + 40 + 128,
	.htotal = 800 + 40 + 128 + 88,
	.vdisplay = 480,
	.vsync_start = 480 + 10,
	.vsync_end = 480 + 10 + 2,
	.vtotal = 480 + 10 + 2 + 33,
	.flags = DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_NVSYNC,
};

static inline struct dsi_crosslink *to_dsi_crosslink(struct drm_panel *panel)
{
	return container_of(panel, struct dsi_crosslink, base);
}

static int dsi_crosslink_disable(struct drm_panel *panel)
{
	struct dsi_crosslink *p = to_dsi_crosslink(panel);

	if (!p->enabled)
		return 0;

	pr_debug("disable panel\n");

	p->enabled = false;

	return 0;
}

static int dsi_crosslink_mipi_write(struct mipi_dsi_device *dsi, u16 reg, u32 val)
{
	u8 msg[] = {
		reg,
		reg >> 8,
		val,
		val >> 8,
		val >> 16,
		val >> 24,
	};

	mipi_dsi_generic_write(dsi, msg, sizeof(msg));

	return 0;
}

static int dsi_crosslink_dsi_init(struct dsi_crosslink *p)
{
	struct mipi_dsi_device *dsi = p->dsi;

	// initialize bridge
	// dsi_crosslink_mipi_write(dsi, POCTRL, 0x8e);
	// dsi_crosslink_mipi_write(dsi, DP0_SRCCTRL, 0x3080);
	// dsi_crosslink_mipi_write(dsi, PXL_PLLPARAM, 0x128301);
	// dsi_crosslink_mipi_write(dsi, PXL_PLLCTRL, 0x1);
	// msleep(100);
	// dsi_crosslink_mipi_write(dsi, PXL_PLLCTRL, 0x5);
	// dsi_crosslink_mipi_write(dsi, SYS_PLLPARAM, 0x110);
	// dsi_crosslink_mipi_write(dsi, DP_PHY_CTRL, 0x1100);
	// dsi_crosslink_mipi_write(dsi, PPI_TX_RX_TA, 0x08000b);
	// dsi_crosslink_mipi_write(dsi, LPTXTIMCNT, 0x7);
	// dsi_crosslink_mipi_write(dsi, D0S_CLRSIPOCOUNT, 0xd);
	// dsi_crosslink_mipi_write(dsi, D1S_CLRSIPOCOUNT, 0xd);
	// dsi_crosslink_mipi_write(dsi, D2S_CLRSIPOCOUNT, 0xd);
	// dsi_crosslink_mipi_write(dsi, D3S_CLRSIPOCOUNT, 0xd);
	// dsi_crosslink_mipi_write(dsi, PPI_LANEENABLE, 0x3);
	// dsi_crosslink_mipi_write(dsi, DSI_LANEENABLE, 0x3);
	// dsi_crosslink_mipi_write(dsi, STARTPPI, 0x1);
	// dsi_crosslink_mipi_write(dsi, STARTDSI, 0x1);
	// dsi_crosslink_mipi_write(dsi, VPCTRL, 0xa00100);
	// dsi_crosslink_mipi_write(dsi, HTIM01, 0x2f001f);
	// dsi_crosslink_mipi_write(dsi, VTIM01, 0x30004);
	// dsi_crosslink_mipi_write(dsi, VFUEN0, 0x1);
	// dsi_crosslink_mipi_write(dsi, SYSCTRL, 0x1);

	return 0;
}

static int dsi_crosslink_enable(struct drm_panel *panel)
{
	struct dsi_crosslink *p = to_dsi_crosslink(panel);

	if (p->enabled)
		return 0;

	pr_debug("panel enable\n");

	dsi_crosslink_dsi_init(p);

	p->enabled = true;

	return 0;
}

static int dsi_crosslink_get_modes(struct drm_panel *panel, struct drm_connector *connector)
{
	struct drm_display_mode *mode = drm_mode_create(connector->dev);
	u32 bus_flags = 0;

	/* Take default timing if there is not device tree node for */
	if (of_get_drm_display_mode(panel->dev->of_node, mode, &bus_flags, 0) < 0) {
		mode = drm_mode_duplicate(connector->dev, &dsi_crosslink_default_mode);
		if (!mode) {
			DRM_ERROR("failed to add mode %ux%ux\n",
				dsi_crosslink_default_mode.hdisplay, dsi_crosslink_default_mode.vdisplay);
			return -ENOMEM;
		}
	}

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode);

	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;

	return 1;
}

static int dsi_crosslink_get_timings(struct drm_panel *panel,
				unsigned int num_timings,
				struct display_timing *timings)
{
	struct dsi_crosslink *p = to_dsi_crosslink(panel);
	unsigned int i;

	if (!p->desc)
		return 0;

	if (p->desc->num_timings < num_timings)
		num_timings = p->desc->num_timings;

	if (timings)
		for (i = 0; i < num_timings; i++)
			timings[i] = p->desc->timings[i];

	return p->desc->num_timings;
}

static const struct drm_panel_funcs dsi_crosslink_funcs = {
	.disable = dsi_crosslink_disable,
	.enable = dsi_crosslink_enable,
	.get_modes = dsi_crosslink_get_modes,
	.get_timings = dsi_crosslink_get_timings,
};

static int dsi_crosslink_mipi_probe(struct mipi_dsi_device *dsi, const struct panel_desc *desc)
{
	struct dsi_crosslink *panel;
	struct device *dev = &dsi->dev;

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;

	panel->enabled = false;
	panel->desc = desc;
	panel->dev = dev;
	panel->dsi = dsi;

	drm_panel_init(&panel->base, dev, &dsi_crosslink_funcs, DRM_MODE_CONNECTOR_DSI);
	panel->base.dev = dev;
	panel->base.funcs = &dsi_crosslink_funcs;

	drm_panel_add(&panel->base);

	dev_set_drvdata(dev, panel);

	return 0;
}

static void dsi_crosslink_remove(struct device *dev)
{
	struct dsi_crosslink *panel = dev_get_drvdata(dev);

	drm_panel_remove(&panel->base);

	dsi_crosslink_disable(&panel->base);
}

static void dsi_crosslink_shutdown(struct device *dev)
{
	struct dsi_crosslink *panel = dev_get_drvdata(dev);

	dsi_crosslink_disable(&panel->base);
}

struct bridge_desc {
	struct panel_desc desc;

	unsigned long flags;
	enum mipi_dsi_pixel_format format;
	unsigned int lanes;
};

static const struct bridge_desc dsi_crosslink_bridge = {
	.desc = {
		.modes = &dsi_crosslink_default_mode,
		.num_modes = 1,
		.bpc = 8,
		.size = {
			.width = 154,
			.height = 86,
		},
		.bus_format = MEDIA_BUS_FMT_RGB888_1X24,
	},
	.flags = MIPI_DSI_MODE_VIDEO |
		 MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
		 MIPI_DSI_MODE_LPM,
	.format = MIPI_DSI_FMT_RGB888,
	.lanes = 1,
};

static const struct of_device_id dsi_of_match[] = {
	{
		.compatible = "pco,dsi_crosslinkxbg",
		.data = &dsi_crosslink_bridge
	},
	{
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, dsi_of_match);

static int dsi_crosslink_dsi_probe(struct mipi_dsi_device *dsi)
{
	const struct bridge_desc *desc;
	const struct of_device_id *id;
	const struct panel_desc *pdesc;
	int err;

	id = of_match_node(dsi_of_match, dsi->dev.of_node);
	if (!id)
		return -ENODEV;

	desc = id->data;

	if (desc) {
		dsi->mode_flags = desc->flags;
		dsi->format = desc->format;
		dsi->lanes = desc->lanes;
		pdesc = &desc->desc;
	} else {
		pdesc = NULL;
	}

	err = dsi_crosslink_mipi_probe(dsi, pdesc);

	if (err < 0)
		return err;

	return mipi_dsi_attach(dsi);
}

static void dsi_crosslink_dsi_remove(struct mipi_dsi_device *dsi)
{
	int err;

	err = mipi_dsi_detach(dsi);
	if (err < 0)
		dev_err(&dsi->dev, "failed to detach from DSI host: %d\n", err);

	dsi_crosslink_remove(&dsi->dev);
}

static void dsi_crosslink_dsi_shutdown(struct mipi_dsi_device *dsi)
{
	dsi_crosslink_shutdown(&dsi->dev);
}

static struct mipi_dsi_driver dsi_crosslink_dsi_driver = {
	.driver = {
		.name = "dsi_crosslink",
		.of_match_table = dsi_of_match,
	},
	.probe = dsi_crosslink_dsi_probe,
	.remove = dsi_crosslink_dsi_remove,
	.shutdown = dsi_crosslink_dsi_shutdown,
};

static int __init dsi_crosslink_init(void)
{
	int err;

	if (IS_ENABLED(CONFIG_DRM_MIPI_DSI)) {
		err = mipi_dsi_driver_register(&dsi_crosslink_dsi_driver);
		if (err < 0)
			return err;
	}

	return 0;
}
module_init(dsi_crosslink_init);

static void __exit dsi_crosslink_exit(void)
{
	if (IS_ENABLED(CONFIG_DRM_MIPI_DSI))
		mipi_dsi_driver_unregister(&dsi_crosslink_dsi_driver);
}
module_exit(dsi_crosslink_exit);

MODULE_AUTHOR("jarsulk <a@a.a>");
MODULE_DESCRIPTION("I.MX 8M Plus DSI - Crosslink - OLED driver");
MODULE_LICENSE("GPL");
