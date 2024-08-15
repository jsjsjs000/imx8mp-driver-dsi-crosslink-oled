// SPDX-License-Identifier: GPL-2.0
/*
 * based on: drivers/gpu/drm/bridge/tc358867xbg.c
 * and: drivers/gpu/drm/panel/panel-rpi-7inch.c
 * and: drivers/gpu/drm/bridge/tc358767.c
 */

#include <linux/delay.h>
#include <linux/media-bus-format.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>
#include <drm/drm_print.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>
#include <video/of_videomode.h>

#include "imx8mp-dsi-crosslink-oled-i2c.h"

#define TAG "dsi_oled_driver: "

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
		// https://www.kernel.org/doc/html/v4.8/gpu/drm-kms.html
	.clock = 40000,     /* OLED SVGA060 804x604 RGB */
	.hdisplay = 800,
	.hsync_start = 800 + 40,				// H FP = 40
	.hsync_end = 800 + 40 + 128,		// H Sync = 128
	.htotal = 800 + 40 + 128 + 88,	// H BP = 88
	.vdisplay = 600,
	.vsync_start = 600 + 1,					// V FP = 1
	.vsync_end = 600 + 1 + 4,				// V Sync = 4
	.vtotal = 600 + 1 + 4 + 23,			// V BP = 23
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

static int dsi_crosslink_enable(struct drm_panel *panel)
{
	struct dsi_crosslink *p = to_dsi_crosslink(panel);
	pr_info(TAG "enabled %d", p->enabled);
	if (p->enabled)
		return 0;

	pr_info(TAG "panel enable\n");

	p->enabled = true;
	return 0;
}

static int dsi_crosslink_get_modes(struct drm_panel *panel, struct drm_connector *connector)
{
	struct drm_display_mode *mode = drm_mode_create(connector->dev);
	u32 bus_flags = 0;

	pr_info(TAG "get modes");
		/* Take default timing if there is not device tree node for */
if (of_get_drm_display_mode(panel->dev->of_node, mode, &bus_flags, 0) < 0)
	pr_err(TAG "------------------- of_get_drm_display_mode err");
else
	pr_err(TAG "------------------- of_get_drm_display_mode ok");
pr_err(TAG "--------- mode: %s\n", mode->name);
pr_err(TAG "--------- clock: %d\n", mode->clock);
pr_err(TAG "--------- hdisplay: %d\n", mode->hdisplay);
pr_err(TAG "--------- hsync_start: %d\n", mode->hsync_start);
pr_err(TAG "--------- hsync_end: %d\n", mode->hsync_end);
pr_err(TAG "--------- htotal: %d\n", mode->htotal);
pr_err(TAG "--------- hskew: %d\n", mode->hskew);
pr_err(TAG "--------- vdisplay: %d\n", mode->vdisplay);
pr_err(TAG "--------- vsync_start: %d\n", mode->vsync_start);
pr_err(TAG "--------- vsync_end: %d\n", mode->vsync_end);
pr_err(TAG "--------- vtotal: %d\n", mode->vtotal);
pr_err(TAG "--------- vscan: %d\n", mode->vscan);


// 	{
// 		mode = drm_mode_duplicate(connector->dev, &dsi_crosslink_default_mode);
// // pr_info(TAG "get mode = %d", !!mode);
// 		if (!mode)
// 		{
// 			DRM_ERROR("failed to add mode %ux%ux\n",
// 				dsi_crosslink_default_mode.hdisplay, dsi_crosslink_default_mode.vdisplay);
// 			return -ENOMEM;
// 		}
// 	}

	drm_mode_set_name(mode);
	pr_info(TAG "set mode %dx%d", mode->hdisplay, mode->vdisplay);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode);

pr_err(TAG "--------- modew: %d\n", mode->width_mm);
pr_err(TAG "--------- modeh: %d\n", mode->width_mm);
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;

	return 1;
}

static int dsi_crosslink_get_timings(struct drm_panel *panel,
		unsigned int num_timings, struct display_timing *timings)
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
		.bus_format = MEDIA_BUS_FMT_RGB888_1X24, /* only DRM_BUS_FLAG_DE_LOW */
	},
	.flags = MIPI_DSI_MODE_VIDEO |
		 MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
		 MIPI_DSI_MODE_VIDEO_HSE,
	.format = MIPI_DSI_FMT_RGB888,
	.lanes = 4,
};

static const struct of_device_id dsi_of_match[] = {
	{
		.compatible = "pco,dsi_crosslink_oled_svga060",
		.data = &dsi_crosslink_bridge
	},
	{}
};
MODULE_DEVICE_TABLE(of, dsi_of_match);

static int dsi_crosslink_dsi_probe(struct mipi_dsi_device *dsi)
{
	const struct bridge_desc *desc;
	const struct of_device_id *id;
	const struct panel_desc *pdesc;
	int err;

	pr_info(TAG "probe\n");
	id = of_match_node(dsi_of_match, dsi->dev.of_node);
	if (!id)
		return -ENODEV;

	desc = id->data;

	if (desc)
	{
		dsi->mode_flags = desc->flags;
		dsi->format = desc->format;
		dsi->lanes = desc->lanes;
		pdesc = &desc->desc;
	} else {
		pdesc = NULL;
	}

	err = dsi_crosslink_mipi_probe(dsi, pdesc);
	if (err < 0)
	{
		pr_err(TAG "probe, err=%d\n", err);
		return err;
	}

	err = mipi_dsi_attach(dsi);
	if (err < 0)
		return err;

	// i2c_driver_write_registers();

	return 0;
}

static int dsi_crosslink_dsi_remove(struct mipi_dsi_device *dsi)
{
	int err;

	pr_info(TAG "remove\n");
	err = mipi_dsi_detach(dsi);
	if (err < 0)
		dev_err(&dsi->dev, "DSI failed to detach from DSI host: %d\n", err);

	dsi_crosslink_remove(&dsi->dev);
	return err;
}

static void dsi_crosslink_dsi_shutdown(struct mipi_dsi_device *dsi)
{
pr_info(TAG "shutdown\n");
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

	pr_info(TAG "initinalize\n");

	// if (i2c_driver_init() < 0)
	// {
	// 	pr_err("i2c_add_driver() failed\n");
	// 	return err;
	// }

	if (IS_ENABLED(CONFIG_DRM_MIPI_DSI))
	{
		err = mipi_dsi_driver_register(&dsi_crosslink_dsi_driver);
		if (err < 0)
		{
			pr_err(TAG "initialize err = %d\n", err);
			return err;
		}
	}

	return 0;
}
module_init(dsi_crosslink_init);

static void __exit dsi_crosslink_exit(void)
{
	pr_info(TAG "deinitialize\n");
	// i2c_driver_exit();

	if (IS_ENABLED(CONFIG_DRM_MIPI_DSI))
	{
		mipi_dsi_driver_unregister(&dsi_crosslink_dsi_driver);
	}
}
module_exit(dsi_crosslink_exit);

MODULE_AUTHOR("jarsulk, PCO");
MODULE_DESCRIPTION("I.MX 8M Plus DSI - Crosslink - OLED driver");
MODULE_LICENSE("GPL");
