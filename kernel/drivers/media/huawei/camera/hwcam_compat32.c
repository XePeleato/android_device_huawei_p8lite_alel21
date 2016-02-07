

#include "hwcam_compat32.h"
#include "cam_log.h"
#include "hwcam_intf.h"

long compat_get_v4l2_event_data(struct v4l2_event *kp, struct v4l2_event32 __user *up)
{
	long ret = 0;

	ret  = get_user(kp->type, &up->type);
	ret |= copy_in_user(&kp->u, &up->u, sizeof(kp->u));
	ret |= get_user(kp->pending, &up->pending);
	ret |= get_user(kp->sequence, &up->sequence);
	ret |= get_compat_timespec(&kp->timestamp, &up->timestamp);
	ret |= get_user(kp->id, &up->id);
	ret |= copy_in_user(kp->reserved, up->reserved, 8 * sizeof(__u32));

	return ret;
}
 
long compat_put_v4l2_event_data(struct v4l2_event *kp, struct v4l2_event32 __user *up)
{
	long ret = 0;
	ret  = put_user(kp->type, &up->type);
	ret |= copy_in_user(&up->u, &kp->u, sizeof(kp->u));
	ret |= put_user(kp->pending, &up->pending);
	ret |= put_user(kp->sequence, &up->sequence);
	ret |= put_compat_timespec(&kp->timestamp, &up->timestamp);
	ret |= put_user(kp->id, &up->id);
	ret |= copy_in_user(up->reserved, kp->reserved, 8 * sizeof(__u32));
	return ret;
}

 static int compat_get_v4l2_window(struct v4l2_window *kp, struct v4l2_window32 __user *up)
{
	if (!access_ok(VERIFY_READ, up, sizeof(struct v4l2_window32)) ||
		copy_in_user(&kp->w, &up->w, sizeof(up->w)) ||
		get_user(kp->field, &up->field) ||
		get_user(kp->chromakey, &up->chromakey) ||
		get_user(kp->clipcount, &up->clipcount))
			return -EFAULT;
	if (kp->clipcount > 2048)
		return -EINVAL;
	if (kp->clipcount) {
		struct v4l2_clip32 __user *uclips;
		struct v4l2_clip __user *kclips;
		int n = kp->clipcount;
		compat_caddr_t p;

		if (get_user(p, &up->clips))
			return -EFAULT;
		uclips = compat_ptr(p);
		kclips = compat_alloc_user_space(n * sizeof(struct v4l2_clip));
		kp->clips = kclips;
		while (--n >= 0) {
			if (copy_in_user(&kclips->c, &uclips->c, sizeof(uclips->c)))
				return -EFAULT;
			if (put_user(n ? kclips + 1 : NULL, &kclips->next))
				return -EFAULT;
			uclips += 1;
			kclips += 1;
		}
	} else
		kp->clips = NULL;
	return 0;
}

static int compat_put_v4l2_window(struct v4l2_window *kp, struct v4l2_window32 __user *up)
{
	if (copy_in_user(&up->w, &kp->w, sizeof(kp->w)) ||
		put_user(kp->field, &up->field) ||
		put_user(kp->chromakey, &up->chromakey) ||
		put_user(kp->clipcount, &up->clipcount))
			return -EFAULT;
	return 0;
}

long compat_get_v4l2_format_data(struct v4l2_format *kp, struct v4l2_format32 __user *up)
{
 	long ret = 0;
	
	if (get_user(kp->type, &up->type))
		return -EFAULT;

	if (0 == kp->type) {
		printk(KERN_INFO "%s: hwcam type is 0, return ok \n", __func__);
		return 0;
	}
	
	switch (kp->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		ret |= copy_in_user(&kp->fmt.pix, &up->fmt.pix, sizeof(struct v4l2_pix_format));
		break;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		ret |= copy_in_user(&kp->fmt.pix_mp, &up->fmt.pix_mp, sizeof(struct v4l2_pix_format_mplane));
		break;
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY:
		ret |= compat_get_v4l2_window(&kp->fmt.win, &up->fmt.win);
		break;
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
		ret |= copy_in_user(&kp->fmt.vbi, &up->fmt.vbi, sizeof(struct v4l2_vbi_format));
		break;
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		ret |= copy_in_user(&kp->fmt.sliced, &up->fmt.sliced, sizeof(struct v4l2_sliced_vbi_format));
		break;
	default:
		printk(KERN_INFO "compat_ioctl32: unexpected VIDIOC_FMT type %d\n",kp->type);
		ret = -EINVAL;
	}
	return ret;
}

long compat_put_v4l2_format_data(struct v4l2_format *kp, struct v4l2_format32 __user *up)
{
 	long ret = 0;

	ret = put_user(kp->type, &up->type);
 	switch (kp->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		ret |= copy_in_user(&up->fmt.pix, &kp->fmt.pix, sizeof(struct v4l2_pix_format));
		break;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		ret |= copy_in_user(&up->fmt.pix_mp, &kp->fmt.pix_mp, sizeof(struct v4l2_pix_format_mplane));
		break;
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY:
		ret |= compat_put_v4l2_window(&kp->fmt.win, &up->fmt.win);
		break;
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
		ret |= copy_in_user(&up->fmt.vbi, &kp->fmt.vbi, sizeof(struct v4l2_vbi_format));
		break;
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		ret |= copy_in_user(&up->fmt.sliced, &kp->fmt.sliced, sizeof(struct v4l2_sliced_vbi_format));
		break;
	default:
		printk(KERN_INFO "compat_ioctl32: unexpected VIDIOC_FMT type %d\n",kp->type);
		ret = -EINVAL;
	}
	return ret;
}

long compat_get_hwisp_stream_buf_info(hwisp_stream_buf_info_t *kp, hwisp_stream_buf_info_t32 __user *up)
{
	long ret = 0;

	ret  = get_user(kp->user_buffer_handle, &up->user_buffer_handle);
	ret |= get_user(kp->y_addr_phy, &up->y_addr_phy);
	ret |= get_user(kp->u_addr_phy, &up->u_addr_phy);
	ret |= get_user(kp->v_addr_phy, &up->v_addr_phy);
	ret |= get_user(kp->y_addr_iommu, &up->y_addr_iommu);
	ret |= get_user(kp->u_addr_iommu, &up->u_addr_iommu);
	ret |= get_user(kp->v_addr_iommu, &up->v_addr_iommu);
	ret |= get_user(kp->ion_fd, &up->ion_fd);
	ret |= get_user(kp->ion_vc_hdl, &up->ion_vc_hdl);
	ret |= get_user(kp->ion_vaddr, &up->ion_vaddr);
	ret |= get_compat_timeval(&kp->timestamp, &up->timestamp);
	ret |= copy_in_user(&kp->port, &up->port, sizeof(ovisp23_port_info_t));

	return ret;
}

long compat_put_hwisp_stream_buf_info(hwisp_stream_buf_info_t *kp, hwisp_stream_buf_info_t32 __user *up)
{
	long ret = 0;
	ret  = put_user(kp->user_buffer_handle, &up->user_buffer_handle);
	ret |= put_user(kp->y_addr_phy, &up->y_addr_phy);
	ret |= put_user(kp->u_addr_phy, &up->u_addr_phy);
	ret |= put_user(kp->v_addr_phy, &up->v_addr_phy);
	ret |= put_user(kp->y_addr_iommu, &up->y_addr_iommu);
	ret |= put_user(kp->u_addr_iommu, &up->u_addr_iommu);
	ret |= put_user(kp->v_addr_iommu, &up->v_addr_iommu);
	ret |= put_user(kp->ion_fd, &up->ion_fd);
	ret |= put_user(kp->ion_vc_hdl, &up->ion_vc_hdl);
	ret |= put_user(kp->ion_vaddr, &up->ion_vaddr);
	ret |= put_compat_timeval(&kp->timestamp, &up->timestamp);
	ret |= copy_in_user(&up->port, &kp->port, sizeof(ovisp23_port_info_t));
	return ret;
}

long compat_get_hwcam_buf_status_data(hwcam_buf_status_t *kp, hwcam_buf_status_t32 __user *up)
{
	long ret = 0;

	ret  = get_user(kp->id, &up->id);
	ret |= get_user(kp->buf_status, &up->buf_status);
	ret |= get_compat_timeval(&kp->tv, &up->tv);

	return ret;
}

long compat_put_hwcam_buf_status_data(hwcam_buf_status_t *kp, hwcam_buf_status_t32 __user *up)
{
	long ret = 0;

	ret  = put_user(kp->id, &up->id);
	ret |= put_user(kp->buf_status, &up->buf_status);
	ret |= put_compat_timeval(&kp->tv, &up->tv);

	return ret;
}


