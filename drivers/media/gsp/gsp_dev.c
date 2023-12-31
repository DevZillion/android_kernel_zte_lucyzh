/*
 * Copyright (C) 2015 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/list.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/pm_runtime.h>
#include <video/gsp_cfg.h>
#include <uapi/video/gsp_r1p1_cfg.h>
#include "gsp_core.h"
#include "gsp_dev.h"
#include "gsp_debug.h"
#include "gsp_interface.h"
#include "gsp_kcfg.h"
#include "gsp_sync.h"
#include "gsp_sysfs.h"
#include "gsp_workqueue.h"
#include "gsp_r1p1/gsp_r1p1_core.h"
#include "gsp_r3p0/gsp_r3p0_core.h"
#include "gsp_r4p0/gsp_r4p0_core.h"
#include "gsp_lite_r1p0/gsp_lite_r1p0_core.h"
#include "gsp_r5p0/gsp_r5p0_core.h"
#include "gsp_lite_r2p0/gsp_lite_r2p0_core.h"
#include "gsp_r6p0/gsp_r6p0_core.h"
#include "gsp_lite_r3p0/gsp_lite_r3p0_core.h"
#include <linux/compat.h>

static struct gsp_core_ops gsp_r1p1_core_ops = {
	.parse_dt = gsp_r1p1_core_parse_dt,
	.alloc = gsp_r1p1_core_alloc,
	.init = gsp_r1p1_core_init,
	.copy = gsp_r1p1_core_copy_cfg,
	.trigger = gsp_r1p1_core_trigger,
	.release = gsp_r1p1_core_release,
	.enable = gsp_r1p1_core_enable,
	.disable = gsp_r1p1_core_disable,
	.intercept = gsp_r1p1_core_intercept,
	.reset = gsp_r1p1_core_reset,
	.dump = gsp_r1p1_core_dump,
};

static struct gsp_core_ops gsp_r3p0_core_ops = {
	.parse_dt = gsp_r3p0_core_parse_dt,
	.alloc = gsp_r3p0_core_alloc,
	.init = gsp_r3p0_core_init,
	.copy = gsp_r3p0_core_copy_cfg,
	.trigger = gsp_r3p0_core_trigger,
	.release = gsp_r3p0_core_release,
	.enable = gsp_r3p0_core_enable,
	.disable = gsp_r3p0_core_disable,
	.intercept = gsp_r3p0_core_intercept,
	.reset = gsp_r3p0_core_reset,
	.dump = gsp_r3p0_core_dump,
};

static struct gsp_core_ops gsp_r4p0_core_ops = {
	.parse_dt = gsp_r4p0_core_parse_dt,
	.alloc = gsp_r4p0_core_alloc,
	.init = gsp_r4p0_core_init,
	.copy = gsp_r4p0_core_copy_cfg,
	.trigger = gsp_r4p0_core_trigger,
	.release = gsp_r4p0_core_release,
	.enable = gsp_r4p0_core_enable,
	.disable = gsp_r4p0_core_disable,
	.intercept = gsp_r4p0_core_intercept,
	.reset = gsp_r4p0_core_reset,
	.dump = gsp_r4p0_core_dump,
};

static struct gsp_core_ops gsp_lite_r1p0_core_ops = {
	.parse_dt = gsp_lite_r1p0_core_parse_dt,
	.alloc = gsp_lite_r1p0_core_alloc,
	.init = gsp_lite_r1p0_core_init,
	.copy = gsp_lite_r1p0_core_copy_cfg,
	.trigger = gsp_lite_r1p0_core_trigger,
	.release = gsp_lite_r1p0_core_release,
	.enable = gsp_lite_r1p0_core_enable,
	.disable = gsp_lite_r1p0_core_disable,
	.intercept = gsp_lite_r1p0_core_intercept,
	.reset = gsp_lite_r1p0_core_reset,
	.dump = gsp_lite_r1p0_core_dump,
};

static struct gsp_core_ops gsp_r5p0_core_ops = {
	.parse_dt = gsp_r5p0_core_parse_dt,
	.alloc = gsp_r5p0_core_alloc,
	.init = gsp_r5p0_core_init,
	.copy = gsp_r5p0_core_copy_cfg,
	.trigger = gsp_r5p0_core_trigger,
	.release = gsp_r5p0_core_release,
	.enable = gsp_r5p0_core_enable,
	.disable = gsp_r5p0_core_disable,
	.intercept = gsp_r5p0_core_intercept,
	.reset = gsp_r5p0_core_reset,
	.dump = gsp_r5p0_core_dump,
};

static struct gsp_core_ops gsp_lite_r2p0_core_ops = {
	.parse_dt = gsp_lite_r2p0_core_parse_dt,
	.alloc = gsp_lite_r2p0_core_alloc,
	.init = gsp_lite_r2p0_core_init,
	.copy = gsp_lite_r2p0_core_copy_cfg,
	.trigger = gsp_lite_r2p0_core_trigger,
	.release = gsp_lite_r2p0_core_release,
	.enable = gsp_lite_r2p0_core_enable,
	.disable = gsp_lite_r2p0_core_disable,
	.intercept = gsp_lite_r2p0_core_intercept,
	.reset = gsp_lite_r2p0_core_reset,
	.dump = gsp_lite_r2p0_core_dump,
};

static struct gsp_core_ops gsp_r6p0_core_ops = {
	.parse_dt = gsp_r6p0_core_parse_dt,
	.alloc = gsp_r6p0_core_alloc,
	.init = gsp_r6p0_core_init,
	.copy = gsp_r6p0_core_copy_cfg,
	.trigger = gsp_r6p0_core_trigger,
	.release = gsp_r6p0_core_release,
	.enable = gsp_r6p0_core_enable,
	.disable = gsp_r6p0_core_disable,
	.intercept = gsp_r6p0_core_intercept,
	.reset = gsp_r6p0_core_reset,
	.dump = gsp_r6p0_core_dump,
};

static struct gsp_core_ops gsp_lite_r3p0_core_ops = {
	.parse_dt = gsp_lite_r3p0_core_parse_dt,
	.alloc = gsp_lite_r3p0_core_alloc,
	.init = gsp_lite_r3p0_core_init,
	.copy = gsp_lite_r3p0_core_copy_cfg,
	.trigger = gsp_lite_r3p0_core_trigger,
	.release = gsp_lite_r3p0_core_release,
	.enable = gsp_lite_r3p0_core_enable,
	.disable = gsp_lite_r3p0_core_disable,
	.intercept = gsp_lite_r3p0_core_intercept,
	.reset = gsp_lite_r3p0_core_reset,
	.dump = gsp_lite_r3p0_core_dump,
};

static struct of_device_id gsp_dt_ids[] = {
	{.compatible = "sprd,gsp-r1p1-sc9830",
	 .data = (void *)&gsp_r1p1_core_ops},
	{.compatible = "sprd,gsp-r3p0-whale2",
	 .data = (void *)&gsp_r3p0_core_ops},
	{.compatible = "sprd,gsp-r4p0-iwhale2",
	 .data = (void *)&gsp_r4p0_core_ops},
	{.compatible = "sprd,gsp-lite_r1p0-sc9833",
	 .data = (void *)&gsp_lite_r1p0_core_ops},
	{.compatible = "sprd,gsp-lite_r1p0-sharklj1",
	 .data = (void *)&gsp_lite_r1p0_core_ops},
	{.compatible = "sprd,gsp-r5p0-isharkl2",
	 .data = (void *)&gsp_r5p0_core_ops},
	{.compatible = "sprd,gsp-lite_r2p0-sharkle",
	 .data = (void *)&gsp_lite_r2p0_core_ops},
	{.compatible = "sprd,gsp-lite_r2p0-pike2",
	 .data = (void *)&gsp_lite_r2p0_core_ops},
	{.compatible = "sprd,gsp-r6p0-sharkl3",
	 .data = (void *)&gsp_r6p0_core_ops},
	{.compatible = "sprd,gsp-lite_r3p0-sharkl5",
	 .data = (void *)&gsp_lite_r3p0_core_ops},
	{},
};
MODULE_DEVICE_TABLE(of, gsp_dt_ids);

int gsp_dev_name_cmp(struct gsp_dev *gsp)
{
	return strncmp(gsp->name, GSP_DEVICE_NAME, sizeof(gsp->name));
}

int gsp_dev_verify(struct gsp_dev *gsp)
{
	if (IS_ERR_OR_NULL(gsp)) {
		GSP_ERR("can't verify with null dev\n");
		return -1;
	}

	return gsp_dev_name_cmp(gsp);
}

void gsp_dev_set(struct gsp_dev *gsp, struct platform_device *pdev)
{
	if (gsp_dev_verify(gsp)
	    || IS_ERR_OR_NULL(pdev)) {
		GSP_ERR("dev set params error\n");
		return;
	}

	gsp->dev = &pdev->dev;
	platform_set_drvdata(pdev, gsp);
}

struct gsp_core *
gsp_dev_to_core(struct gsp_dev *gsp, int index)
{
	struct gsp_core *core = NULL;

	if (index < 0
	    || index > gsp->core_cnt) {
		GSP_ERR("index value error\n");
		return core;
	}

	for_each_gsp_core(core, gsp) {
		if (index == 0)
			break;
		index--;
	}

	return core;
}

struct device *gsp_dev_to_device(struct gsp_dev *gsp)
{
	return gsp->dev;
}

int gsp_dev_to_core_cnt(struct gsp_dev *gsp)
{
	return gsp->core_cnt;
}

int gsp_dev_to_io_cnt(struct gsp_dev *gsp)
{
	return gsp->io_cnt;
}

static void gsp_dev_add_core(struct gsp_dev *gsp,
			     struct gsp_core *core)
{
	if (IS_ERR_OR_NULL(gsp)
	    || IS_ERR_OR_NULL(core)) {
		GSP_ERR("add core params error\n");
		return;
	}

	list_add_tail(&core->list, &gsp->cores);
	core->parent = gsp;
}


int gsp_dev_is_idle(struct gsp_dev *gsp)
{
	int ret = 0;
	struct gsp_core *core = NULL;

	for_each_gsp_core(core, gsp) {
		ret = gsp_core_is_idle(core);
		if (!ret)
			break;
	}

	return ret;
}

int gsp_dev_is_suspend(struct gsp_dev *gsp)
{
	int ret = 0;
	struct gsp_core *core = NULL;

	for_each_gsp_core(core, gsp) {
		ret = gsp_core_is_suspend(core);
		if (ret)
			break;
	}

	return ret;
}

int gsp_dev_is_suspending(struct gsp_dev *gsp)
{
	int ret = 0;
	struct gsp_core *core = NULL;

	for_each_gsp_core(core, gsp) {
		ret = gsp_core_suspend_state_get(core);
		if (ret)
			break;
	}

	return ret;

}

struct gsp_core_ops *gsp_dev_to_core_ops(struct gsp_dev *gsp)
{
	const struct of_device_id *id = NULL;
	struct gsp_core_ops *ops = NULL;

	if (IS_ERR_OR_NULL(gsp)) {
		GSP_ERR("dev to core ops params error\n");
		return ops;
	}

	id = of_match_node(gsp_dt_ids, gsp->dev->of_node);
	if (IS_ERR_OR_NULL(id)) {
		GSP_ERR("find core ops failed\n");
		return ops;
	}

	ops = (struct gsp_core_ops *)id->data;

	return ops;
}

struct gsp_interface *gsp_dev_to_interface(struct gsp_dev *gsp)
{
	return gsp->interface;
}

int gsp_dev_prepare(struct gsp_dev *gsp)
{
	int ret = -1;

	if (gsp_dev_verify(gsp)) {
		GSP_ERR("auto gate enable invalidate core\n");
		return ret;
	}

	if (!gsp_interface_is_attached(gsp->interface)) {
		GSP_ERR("gsp has not attached interface\n");
		return ret;
	}

	ret = gsp_interface_init(gsp->interface);
	if (ret)
		GSP_ERR("gsp interface prepare failed\n");

	return ret;
}

void gsp_dev_unprepare(struct gsp_dev *gsp)
{
	int ret = -1;

	if (gsp_dev_verify(gsp)) {
		GSP_ERR("auto gate disable invalidate core\n");
		return;
	}

	if (!gsp_interface_is_attached(gsp->interface)) {
		GSP_ERR("gsp has not attached interface\n");
		return;
	}

	ret = gsp_interface_deinit(gsp->interface);
	if (ret)
		GSP_ERR("gsp interface unprepare failed\n");
}

int gsp_dev_init(struct gsp_dev *gsp)
{
	int ret = -1;
	struct gsp_core *core = NULL;

	if (gsp_dev_verify(gsp))
		return ret;

	ret = gsp_dev_sysfs_init(gsp);
	if (ret) {
		GSP_ERR("gsp sysfs initialize failed\n");
		return ret;
	}

	ret = gsp_interface_attach(&gsp->interface, gsp);
	if (ret) {
		GSP_ERR("gsp interface attach failed\n");
		return ret;
	}

	for_each_gsp_core(core, gsp) {
		if (IS_ERR_OR_NULL(core))
			return ret;

		if (gsp_core_verify(core)) {
			GSP_ERR("dev init core params error\n");
			return ret;
		}

		ret = gsp_core_init(core);
		if (ret) {
			GSP_ERR("init core[%d] failed\n", gsp_core_to_id(core));
			return ret;
		}
		GSP_INFO("initialize core[%d] success\n", gsp_core_to_id(core));
	}

	ret = gsp_dev_prepare(gsp);
	if (ret) {
		GSP_ERR("gsp interface prepare failed\n");
		goto exit;
	}

exit:
	return ret;
}

int gsp_dev_alloc(struct device *dev, struct gsp_dev **gsp)
{
	int ret = -1;
	int i = 0;
	u32 cnt = 0;
	const char *name = NULL;
	struct device_node *np = NULL;
	struct device_node *child = NULL;
	struct gsp_core *core = NULL;
	struct gsp_core_ops *ops = NULL;

	*gsp = devm_kzalloc(dev, sizeof(struct gsp_dev), GFP_KERNEL);
	if (IS_ERR_OR_NULL(gsp)) {
		GSP_ERR("no enough memory for dev\n");
		return ret;
	}
	memset(*gsp, 0, sizeof(struct gsp_dev));
	(*gsp)->dev = dev;
	INIT_LIST_HEAD(&(*gsp)->cores);

	np = dev->of_node;
	ret = of_property_read_u32(np, "core-cnt", &cnt);
	if (ret) {
		GSP_ERR("read core count failed\n");
		return ret;
	}
	GSP_INFO("node count: %d\n", cnt);
	(*gsp)->core_cnt = cnt;

	ret = of_property_read_string(np, "name", &name);
	if (ret) {
		GSP_ERR("read name failed\n");
		return ret;
	}
	strlcpy((*gsp)->name, name, sizeof((*gsp)->name));
	GSP_INFO("gsp device name: %s\n", (*gsp)->name);

	ret = of_property_read_u32(np, "io-cnt", &cnt);
	if (ret) {
		GSP_ERR("read io count failed\n");
		return ret;
	}
	GSP_INFO("io count: %d\n", cnt);
	(*gsp)->io_cnt = cnt;

	ops = gsp_dev_to_core_ops(*gsp);
	if (IS_ERR_OR_NULL(ops)) {
		GSP_ERR("dev to core ops failed\n");
		return ret;
	}

	for (i = 0; i < (*gsp)->core_cnt; i++) {
		child = of_parse_phandle(np, "cores", i);
		if (IS_ERR_OR_NULL(child)) {
			GSP_ERR("parse core[%d] phandle failed\n", i);
			ret = -1;
			break;
		}

		ret = gsp_core_alloc(&core, ops, child);
		if (ret)
			break;
		GSP_INFO("core[%d] allocate success\n", gsp_core_to_id(core));
		gsp_dev_add_core(*gsp, core);
	}

	return ret;
}

int gsp_dev_alloc_and_init(struct device *dev, struct gsp_dev **gsp)
{
	int ret = -1;

	if (IS_ERR_OR_NULL(dev)
	    || IS_ERR_OR_NULL(gsp)) {
		GSP_ERR("dev initialze params error\n");
		return ret;
	}

	ret = gsp_dev_alloc(dev, gsp);
	if (ret) {
		GSP_ERR("dev alloc failed\n");
		return ret;
	}

	ret = gsp_dev_init(*gsp);
	if (ret)
		GSP_ERR("dev internal init failed\n");

	return ret;
}

int gsp_dev_get_capability(struct gsp_dev *gsp,
		       struct gsp_capability **capa)
{
	struct gsp_core *core = NULL;
	struct gsp_capability *capability = NULL;

	if (gsp_dev_verify(gsp)) {
		GSP_ERR("get capability with null dev or ops\n");
		return -1;
	}

	/* here set default value to zero */
	core = gsp_dev_to_core(gsp, 0);
	if (IS_ERR_OR_NULL(core)
	    || IS_ERR_OR_NULL(core->ops)) {
		GSP_ERR("core ops has not been initialized\n");
		return -1;
	}

	capability = gsp_core_get_capability(core);
	if (IS_ERR_OR_NULL(capa)) {
		GSP_ERR("get core[0] capability failed\n");
		return -1;
	}

	(*capa) = capability;
	/*
	 * io_cnt attribute couples with gsp_dev
	 * so we assign thie attribute here
	 */
	(*capa)->io_cnt = gsp_dev_to_io_cnt(gsp);
	(*capa)->core_cnt = gsp_dev_to_core_cnt(gsp);
	return 0;
}

void gsp_dev_start_work(struct gsp_dev *gsp)
{
	struct gsp_core *core = NULL;

	for_each_gsp_core(core, gsp) {
		if (!gsp_core_is_idle(core))
			continue;
		GSP_DEBUG("gsp core[%d] is idle to start work\n",
			  gsp_core_to_id(core));
		gsp_core_work(core);
	}
}

int gsp_dev_resume_wait(struct gsp_dev *gsp)
{
	struct gsp_core *core = NULL;
	long time = 0;
	int resume_status = 0;

	for_each_gsp_core(core, gsp) {
		if (!gsp_core_suspend_state_get(core))
			continue;

		time = wait_for_completion_interruptible_timeout
				(&core->resume_done, GSP_CORE_RESUME_WAIT);
		if (-ERESTARTSYS == time) {
			GSP_ERR("core[%d] resume interrupt when resume wait\n",
				gsp_core_to_id(core));
			resume_status = -1;
		} else if (0 == time) {
			GSP_ERR("core[%d] resume wait timeout\n",
				gsp_core_to_id(core));
			resume_status = -1;
		} else if (time > 0) {
			GSP_DEBUG("core[%d] resume wait success\n",
				  gsp_core_to_id(core));
		}

		if (resume_status == 0)
			reinit_completion(&core->resume_done);
		else {
			GSP_ERR("resume wait done fail!\n");
			break;
		}
	}

	return resume_status;
}


int gsp_dev_open(struct inode *node, struct file *file)
{
	struct gsp_dev *gsp = NULL;
	struct misc_dev *misc = NULL;

	GSP_INFO("open gsp device\n");
	misc = file->private_data;
	if (IS_ERR_OR_NULL(misc)) {
		GSP_ERR("gsp has no misc device\n");
		return -EINVAL;
	}

	gsp = container_of(file->private_data, struct gsp_dev, mdev);
	if (gsp_dev_verify(gsp)) {
		GSP_ERR("open error gsp device\n");
		return -EINVAL;
	}

	file->private_data = gsp;

	return 0;
}

ssize_t gsp_dev_read(struct file *file, char __user *data,
		     size_t cnt, loff_t *off)
{
	/* to add print core information */
	return 1;
}

ssize_t gsp_dev_write(struct file *file, const char __user *data,
		      size_t cnt, loff_t *off)
{
	/* to add reset gsp device interface */
	return 1;
}

static long gsp_dev_ioctl(struct file *file, unsigned int cmd,
			  unsigned long arg)
{
	int ret = -1;
	int ctrl_code = _IOC_NR(cmd);
	size_t size = _IOC_SIZE(cmd);
	int cnt = 0;
	bool async = false;
	bool split = false;
	struct gsp_dev *gsp = NULL;
	struct gsp_capability *capa;
	struct gsp_kcfg_list kcfg_list;

	if (_IOC_TYPE(cmd) != GSP_IO_MAGIC) {
		ret = -ENOTTY;
		goto exit;
	}

	gsp = file->private_data;
	if (gsp_dev_verify(gsp)) {
		GSP_ERR("ioctl with err dev\n");
		ret = -EBADF;
		goto exit;
	}

	switch (ctrl_code & GSP_IO_MASK) {
	case GSP_GET_CAPABILITY:
		if (size < sizeof(*capa))
			GSP_ERR("size: %zu less than request: %zu.\n",
				size, sizeof(struct gsp_capability));
		else
			ret = gsp_dev_get_capability(gsp, &capa);

		if (ret) {
			GSP_ERR("get capability error\n");
			goto exit;
		}

		ret = copy_to_user((void __user *)arg,
				   (const void *)capa, size);
		if (ret) {
			GSP_ERR("get capability error\n");
			goto exit;
		}

		break;

	case GSP_TRIGGER:
		async = (_IOC_NR(cmd) & GSP_ASYNC_MASK) ? 1 : 0;
		split = (_IOC_NR(cmd) & GSP_SPLIT_MASK) ? 1 : 0;
		cnt = (_IOC_NR(cmd) & GSP_CNT_MASK);

		GSP_DEBUG("aysnc: %d, split: %d, cnt: %d\n", async, split, cnt);
		if (cnt > GSP_MAX_IO_CNT(gsp)
		    || cnt < 1) {
			GSP_ERR("request error number kcfgs\n");
			ret = -EINVAL;
			goto exit;
		}

		if (size < sizeof(struct gsp_cfg)) {
			GSP_ERR("error base cfg size: %zu\n", size);
			ret = -EINVAL;
			goto exit;
		}

		gsp_kcfg_list_init(&kcfg_list, async, split, size, cnt);

		ret = gsp_kcfg_list_acquire(gsp, &kcfg_list, cnt);
		if (ret) {
			GSP_ERR("kcfg list acuqire failed\n");
			if (gsp_kcfg_list_is_empty(&kcfg_list))
				goto exit;
			else
				goto kcfg_list_put;
		}

		ret = gsp_kcfg_list_fill(&kcfg_list, (void __user *)arg);
		if (ret) {
			GSP_ERR("kcfg list fill failed\n");
			goto kcfg_list_put;
		}

		ret = gsp_kcfg_list_push(&kcfg_list);
		if (ret) {
			GSP_ERR("kcfg list push failed\n");
			goto kcfg_list_release;
		}

		if (gsp_dev_is_suspending(gsp) ||
			gsp_dev_is_suspend(gsp)) {
			pm_runtime_mark_last_busy(gsp->dev);
			pm_runtime_get_sync(gsp->dev);

			if (gsp_dev_resume_wait(gsp))
				goto kcfg_list_release;
			else
				pm_runtime_mark_last_busy(gsp->dev);
		} else {
			pm_runtime_mark_last_busy(gsp->dev);
		}

		if (gsp_dev_is_suspend(gsp))
			GSP_INFO("no need to process kcfg at suspend state\n");

		gsp_dev_start_work(gsp);

		if (!async) {
			ret = gsp_kcfg_list_wait(&kcfg_list);
			if (ret)
				goto kcfg_list_release;
		}
		break;
	default:
		GSP_WARN("unknown cmd\n");
		break;
	}

	goto exit;

kcfg_list_release:
	gsp_kcfg_list_release(&kcfg_list);
kcfg_list_put:
	gsp_kcfg_list_put(&kcfg_list);
exit:
	return ret;
}

#ifdef CONFIG_COMPAT
static long gsp_dev_compat_ioctl(struct file *file, unsigned int cmd,
			  unsigned long arg)
{
	int ret = -1;

	ret = gsp_dev_ioctl(file, cmd, (unsigned long)compat_ptr(arg));
	return ret;
}
#endif

const struct file_operations gsp_dev_fops = {
	.owner = THIS_MODULE,
	.open = gsp_dev_open,
	.read = gsp_dev_read,
	.write = gsp_dev_write,
	.unlocked_ioctl = gsp_dev_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = gsp_dev_compat_ioctl,
#endif
};

static void gsp_miscdev_deregister(struct gsp_dev *gsp)
{
	if (gsp_dev_verify(gsp)) {
		GSP_ERR("misc dev deregister params error\n");
		return;
	}

	misc_deregister(&gsp->mdev);
}

static int gsp_miscdev_register(struct gsp_dev *gsp)
{
	if (gsp_dev_verify(gsp)) {
		GSP_ERR("register null dev\n");
		return -1;
	}

	gsp->mdev.minor = MISC_DYNAMIC_MINOR;
	gsp->mdev.name = "gsp";
	gsp->mdev.fops = &gsp_dev_fops;

	return misc_register(&gsp->mdev);
}

static void gsp_dev_free(struct gsp_dev *gsp)
{
	struct gsp_core *core = NULL;
	struct gsp_core *tmp = NULL;

	if (IS_ERR_OR_NULL(gsp))
		GSP_DEBUG("nothing to do for unalloc gsp\n");

	if (list_empty(&gsp->cores))
		return;

	for_each_gsp_core_safe(core, tmp, gsp) {
		if (!IS_ERR_OR_NULL(core))
			gsp_core_free(core);
	}
}

static void gsp_dev_deinit(struct gsp_dev *gsp)
{
	struct gsp_core *core = NULL;

	if (IS_ERR_OR_NULL(gsp))
		GSP_DEBUG("nothing to do for uninit gsp\n");

	if (list_empty(&gsp->cores))
		return;

	for_each_gsp_core(core, gsp) {
		if (!IS_ERR_OR_NULL(core))
			gsp_core_deinit(core);
	}

	gsp_interface_detach(gsp->interface);

	gsp_dev_sysfs_destroy(gsp);
}

void gsp_dev_free_and_deinit(struct gsp_dev *gsp)
{
	if (IS_ERR_OR_NULL(gsp)) {
		GSP_ERR("dev free and deinit params error\n");
		return;
	}

	gsp_dev_deinit(gsp);

	gsp_dev_free(gsp);
}

static int gsp_dev_probe(struct platform_device *pdev)
{
	struct gsp_dev *gsp = NULL;
	int ret = -1;

	if (IS_ERR_OR_NULL(pdev)) {
		GSP_ERR("probe params error\n");
		goto exit;
	}

	ret = gsp_dev_alloc_and_init(&pdev->dev, &gsp);
	if (ret) {
		GSP_ERR("dev alloc and init failed\n");
		goto dev_free_and_deinit;
	}

	ret = gsp_miscdev_register(gsp);
	if (ret) {
		GSP_ERR("mdev register failed\n");
		goto dev_free_and_deinit;
	}

	gsp_dev_set(gsp, pdev);

	pm_runtime_set_active(&pdev->dev);
	pm_runtime_set_autosuspend_delay(&pdev->dev, PM_RUNTIME_DELAY_MS);
	pm_runtime_use_autosuspend(&pdev->dev);

	pm_runtime_enable(&pdev->dev);

	GSP_INFO("probe success\n");

	return ret;

dev_free_and_deinit:
	gsp_dev_free_and_deinit(gsp);

exit:
	return ret;
}

#if defined(CONFIG_PM_SLEEP) || defined(CONFIG_PM)
static void gsp_dev_wait_suspend(struct gsp_dev *gsp)
{
	struct gsp_core *core = NULL;
	bool need_wait = false;
	int ret = 0;

	for_each_gsp_core(core, gsp) {
		if (gsp_core_verify(core)) {
			GSP_ERR("wait work done params error\n");
			break;
		}

		need_wait = gsp_core_is_idle(core) ? false : true;
		gsp_core_suspend_state_set(core, CORE_STATE_SUSPEND_WAIT);
		if (need_wait)
			ret = gsp_core_suspend_wait(core);

		if (ret == 0)
			gsp_core_suspend(core);
		else
			GSP_ERR("gsp_dev_wait suspend fail !\n");
	}
}

static void gsp_dev_enter_suspending(struct gsp_dev *gsp)
{
	struct gsp_core *core = NULL;

	for_each_gsp_core(core, gsp) {
		if (gsp_core_verify(core)) {
			GSP_ERR("check core verify params error\n");
			break;
		}
		gsp_core_suspend_state_set(core, CORE_STATE_SUSPEND_BEGIN);
	}
}

static void gsp_dev_enter_suspend(struct gsp_dev *gsp)
{
	struct gsp_core *core = NULL;

	for_each_gsp_core(core, gsp) {
		if (gsp_core_verify(core)) {
			GSP_ERR("check core verify params error\n");
			break;
		}

		gsp_core_suspend(core);
	}
}

static int gsp_dev_stop(struct gsp_dev *gsp)
{
	int ret = -1;
	struct gsp_core *core = NULL;

	for_each_gsp_core(core, gsp) {
		if (gsp_core_verify(core)) {
			GSP_ERR("stop device params error\n");
			break;
		}

		ret = gsp_core_stop(core);
		if (ret) {
			GSP_ERR("gsp core stop fail!\n");
			goto exit;
		}
	}
	if (gsp_dev_is_suspend(gsp))
		gsp_dev_unprepare(gsp);

exit:
	return ret;
}
#endif

static int gsp_dev_remove(struct platform_device *pdev)
{
	struct gsp_dev *gsp = NULL;

	GSP_INFO("remove gsp device\n");
	gsp = platform_get_drvdata(pdev);
	if (gsp_dev_verify(gsp)) {
		GSP_ERR("get a error gsp device\n");
		return -EINVAL;
	}

	gsp_miscdev_deregister(gsp);
	gsp_dev_free_and_deinit(gsp);

	return 0;
}

#if defined(CONFIG_PM_SLEEP) || defined(CONFIG_PM)
static int gsp_dev_suspend(struct device *dev)
{
	int ret = -1;
	struct platform_device *pdev = NULL;
	struct gsp_dev *gsp = NULL;

	pdev = to_platform_device(dev);
	gsp = platform_get_drvdata(pdev);

	if (gsp_dev_is_suspend(gsp)) {
		GSP_ERR("gsp dev already suspend, skip !\n");
		return 0;
	}

	gsp_dev_enter_suspending(gsp);

	if (!gsp_dev_is_idle(gsp))
		gsp_dev_wait_suspend(gsp);
	else
		gsp_dev_enter_suspend(gsp);

	ret = gsp_dev_stop(gsp);
	if (ret)
		GSP_ERR("stop device failed\n");

	return 0;
}

static int gsp_dev_resume(struct device *dev)
{
	int ret = 0;
	struct platform_device *pdev = NULL;
	struct gsp_dev *gsp = NULL;
	struct gsp_core *core = NULL;

	pdev = to_platform_device(dev);
	gsp = platform_get_drvdata(pdev);

	for_each_gsp_core(core, gsp) {
		if (gsp_core_verify(core)) {
			GSP_ERR("resume error core\n");
			ret = -1;
			break;
		}
		if (!gsp_core_is_suspend(core)) {
			long time = 0;
			int err = 0;

			time = wait_for_completion_interruptible_timeout
				(&core->suspend_done, GSP_CORE_SUSPEND_WAIT);
			if (-ERESTARTSYS == time) {
				GSP_ERR("core[%d] interrupt in suspend wait\n",
					gsp_core_to_id(core));
				err++;
			} else if (0 == time) {
				GSP_ERR("core[%d] suspend wait timeout\n",
					gsp_core_to_id(core));
				err++;
			} else if (time > 0) {
				GSP_DEBUG("core[%d] suspend wait success\n",
					  gsp_core_to_id(core));
			}

			if (err) {
				ret = -1;
				break;
			}
		}
	}

	if (ret)
		GSP_INFO("resume wait not sucess, force exec resume!\n");

	ret = gsp_dev_prepare(gsp);
	if (ret) {
		GSP_ERR("gsp dev resume prepare fail !\n");
		goto exit;
	}

	for_each_gsp_core(core, gsp) {
		gsp_core_resume(core);
		gsp_core_suspend_state_set(core, CORE_STATE_SUSPEND_EXIT);
		complete(&core->resume_done);
		ret = 0;
	}

exit:
	return ret;
}
#endif

#ifdef CONFIG_PM_SLEEP
static int gsp_dev_pm_suspend(struct device *dev)
{
	int ret;

	if (pm_runtime_suspended(dev))
		ret = 0;
	else
		ret = gsp_dev_suspend(dev);

	return ret;
}

static int gsp_dev_pm_resume(struct device *dev)
{
	int ret;

	if (!pm_runtime_suspended(dev)) {
		ret = gsp_dev_resume(dev);

		pm_runtime_disable(dev);
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);
	} else
		ret = 0;

	return ret;
}

#endif

#ifdef CONFIG_PM
static int gsp_dev_runtime_suspend(struct device *dev)
{
	int ret;

	ret = gsp_dev_suspend(dev);
	return ret;
}

static int gsp_dev_runtime_resume(struct device *dev)
{
	int ret;

	ret = gsp_dev_resume(dev);
	return ret;
}

static int gsp_dev_runtime_idle(struct device *dev)
{
	return 0;
}

#endif

static const struct dev_pm_ops gsp_dev_pmops = {
#ifdef CONFIG_PM_SLEEP
	SET_SYSTEM_SLEEP_PM_OPS(gsp_dev_pm_suspend, gsp_dev_pm_resume)
#endif
#ifdef CONFIG_PM
	SET_RUNTIME_PM_OPS(gsp_dev_runtime_suspend,
					gsp_dev_runtime_resume,
					gsp_dev_runtime_idle)
#endif
};

static struct platform_driver gsp_drv = {
	.probe = gsp_dev_probe,
	.remove = gsp_dev_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "sprd-gsp",
		.of_match_table = of_match_ptr(gsp_dt_ids),
		.pm = &gsp_dev_pmops,
	},
};

static int __init gsp_drv_init(void)
{
	int ret = -1;

	GSP_INFO("gsp device init begin\n");

	ret = platform_driver_register(&gsp_drv);

	if (ret)
		GSP_ERR("gsp platform driver register failed\n");

	GSP_INFO("gsp device init end\n");

	return ret;
}
module_init(gsp_drv_init);

static void __exit gsp_drv_deinit(void)
{
	platform_driver_unregister(&gsp_drv);
}
module_exit(gsp_drv_deinit);

MODULE_AUTHOR("yintian.tao <yintian.tao@spreadtrum.com>");
MODULE_DESCRIPTION("GSP Driver");
MODULE_LICENSE("GPL");
