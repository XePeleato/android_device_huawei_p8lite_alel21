/*
 *  hi3630_hi6402es.c
 *  ALSA SoC
 *  cpu-dai   : hi3630
 *  codec-dai : hi6402es
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/of.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>

static struct snd_soc_dai_link hi3630_hi6402es_dai_link[] = {
	{
		/* dai link name*/
		.name		= "hi3630_hi6402es_pb_normal",
		/* stream name same as name */
		.stream_name	= "hi3630_hi6402es_pb_normal",
		/* codec(hi6402es) device name ,see in hi6402es.c */
		.codec_name	= "hi6402es-codec",
		/* cpu(k3v3:asp) dai name(device name), see in hi3630-pcm.c */
		.cpu_dai_name	= "sio-audio",
		/* codec dai name, see in struct snd_soc_dai_driver in hi6402es.c */
		.codec_dai_name	= "hi6402es-audio-dai",
		/* platform(k3v3:asp) device name, see in hi3630-pcm.c */
		.platform_name	= "hi6210-hifi",
	},
	{
		/* dai link name*/
		.name		= "hi3630_voice",
		/* stream name same as name */
		.stream_name	= "hi3630_voice",
		/* codec(hi6401) device name ,see in hi6401.c */
		.codec_name	= "hi6402es-codec",
		/* cpu(k3v3:asp) dai name(device name), see in hi3630-pcm.c */
		.cpu_dai_name	= "snd-soc-dummy-dai",
		/* codec dai name, see in struct snd_soc_dai_driver in hi6401.c */
		.codec_dai_name	= "hi6402es-voice-dai",
		/* platform(k3v3:asp) device name, see in hi3630-pcm.c */
		.platform_name	= "snd-soc-dummy",
	},
	{
		/* dai link name*/
		.name		= "hi3630_fm1",
		/* stream name same as name */
		.stream_name	= "hi3630_fm1",
		/* codec(hi6401) device name ,see in hi6401.c */
		.codec_name	= "hi6402es-codec",
		/* cpu(k3v3:asp) dai name(device name), see in hi3630-pcm.c */
		.cpu_dai_name	= "snd-soc-dummy-dai",
		/* codec dai name, see in struct snd_soc_dai_driver in hi6401.c */
		.codec_dai_name	= "hi6402es-fm-dai",
		/* platform(k3v3:asp) device name, see in hi3630-pcm.c */
		.platform_name	= "snd-soc-dummy",
	},		
	{
		/* dai link name*/
		.name		= "hi3630_fm2",
		/* stream name same as name */
		.stream_name	= "hi3630_fm2",
		/* codec(hi6401) device name ,see in hi6401.c */
		.codec_name	= "hi6402es-codec",
		/* cpu(k3v3:asp) dai name(device name), see in hi3630-pcm.c */
		.cpu_dai_name	= "snd-soc-dummy-dai",
		/* codec dai name, see in struct snd_soc_dai_driver in hi6401.c */
		.codec_dai_name	= "hi6402es-fm-dai",
		/* platform(k3v3:asp) device name, see in hi3630-pcm.c */
		.platform_name	= "snd-soc-dummy",
	},
	{
		/* dai link name*/
		.name		= "hi3630_hi6402es_pb_dsp",
		/* stream name same as name */
		.stream_name	= "hi3630_hi6402es_pb_dsp",
		/* codec(hi6402es) device name ,see in hi6402es.c */
		.codec_name	= "hi6402es-codec",
		/* cpu(k3v3:asp) dai name(device name), see in hi3630-pcm.c */
		.cpu_dai_name	= "sio-audio",
		/* codec dai name, see in struct snd_soc_dai_driver in hi6402es.c */
		.codec_dai_name	= "hi6402es-audio-dai",
		/* platform(k3v3:asp) device name, see in hi3630-pcm.c */
		.platform_name	= "hi3630-srcup-dsp",
	},
};

/* Audio machine driver */
static struct snd_soc_card hi3630_hi6402es_card = {
	/* sound card name, can see all sound cards in /proc/asound/cards */
	.name = "HI3630_HI6402ES_CARD",
	.owner = THIS_MODULE,
	.dai_link = hi3630_hi6402es_dai_link,
	.num_links = ARRAY_SIZE(hi3630_hi6402es_dai_link),
};

static int hi3630_hi6402es_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct snd_soc_card *card = &hi3630_hi6402es_card;

	card->dev = &pdev->dev;

	ret = snd_soc_register_card(card);
	if (ret)
		pr_err("%s : register failed %d\n", __FUNCTION__, ret);

	return ret;
}

static int hi3630_hi6402es_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);

	if (NULL != card)
		snd_soc_unregister_card(card);

	return 0;
}

static const struct of_device_id hi3630_hi6402es_of_match[] = {
	{.compatible = "hisilicon,hi3630-hi6402es", },
	{ },
};
MODULE_DEVICE_TABLE(of, hi3630_hi6402es_of_match);

static struct platform_driver hi3630_hi6402es_driver = {
	.driver	= {
		.name = "hi3630_hi6402es",
		.owner = THIS_MODULE,
		.of_match_table = hi3630_hi6402es_of_match,
	},
	.probe	= hi3630_hi6402es_probe,
	.remove	= hi3630_hi6402es_remove,
};
module_platform_driver(hi3630_hi6402es_driver);

/* Module information */
MODULE_AUTHOR("chengong <apollo.chengong@huawei.com>");
MODULE_DESCRIPTION("ALSA SoC for Hisilicon Hi3630 with hi6402es codec");
MODULE_LICENSE("GPL");
MODULE_ALIAS("machine driver:hi3630-hi6402es");