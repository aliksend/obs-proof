#include <obs.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

#define blog(level, msg, ...) blog(level, "[obs-goheadless] " msg, ##__VA_ARGS__)

bool load_module(char* module_name) {
	char module_path[256];
	snprintf(module_path, sizeof module_path, "../obs-plugins/%s.so", module_name);
	char module_data_path[256];
	snprintf(module_data_path, sizeof module_path, "../data/obs-plugins/%s", module_name);

	obs_module_t *module;
	int iret = obs_open_module(&module, module_path, module_data_path);
	switch (iret) {
		case MODULE_ERROR:            {
			blog(LOG_ERROR, "Failed to load module %s: A generic error occurred", module_name);
			return FALSE;
		}
		case MODULE_FILE_NOT_FOUND:   {
			blog(LOG_ERROR, "Failed to load module %s: The module was not found", module_name);
			return FALSE;
		}
		case MODULE_MISSING_EXPORTS:  {
			blog(LOG_ERROR, "Failed to load module %s: Required exports are missing", module_name);
			return FALSE;
		}
		case MODULE_INCOMPATIBLE_VER: {
			blog(LOG_ERROR, "Failed to load module %s: Incompatible version", module_name);
			return FALSE;
		}
		default: if (iret != MODULE_SUCCESS) {
			blog(LOG_ERROR, "Failed to load module %s. Unknown error", module_name);
			return FALSE;
		}
	}
	bool bret = obs_init_module(module);
	if (!bret) {
		blog(LOG_ERROR, "obs_init_module for module %s fails", module_name);
		return FALSE;
	}
	return TRUE;
}

int main() {
  //// ======== constants
  int width = 400;
  int height = 300;
  int output_width = 400;
  int output_height = 300;

  //// ======== startup
	printf("[==> Running obs_startup\n");
  bool bret = obs_startup("en-US", NULL, NULL);

	// --- reset audio
	struct obs_audio_info oai;

	oai.samples_per_sec = 44100;
	oai.speakers = SPEAKERS_STEREO;

	printf("[==> Running obs_reset_audio\n");
	bret = obs_reset_audio(&oai);
	if (!bret) {
		blog(LOG_ERROR, "obs_reset_audio fails");
	}

	// --- reset video
	struct obs_video_info ovi;

	// 30 fps
	ovi.fps_num = 30;
	ovi.fps_den = 1;

	ovi.graphics_module = DL_OPENGL; // "libobs-opengl.so.0"
	ovi.base_width = width;
	ovi.base_height = height;
	ovi.output_width = output_width;
	ovi.output_height = output_height;
	ovi.output_format = VIDEO_FORMAT_NV12;
	ovi.colorspace = VIDEO_CS_601;
	ovi.range = VIDEO_RANGE_PARTIAL;
	ovi.adapter = 0; // config_get_uint(App()->GlobalConfig(), "Video", "AdapterIdx");
	ovi.gpu_conversion = false;
	ovi.scale_type = OBS_SCALE_BICUBIC;

	printf("[==> Running obs_reset_video\n");
	int iret = obs_reset_video(&ovi);
	switch (iret) {
		case OBS_VIDEO_MODULE_NOT_FOUND:
		{
			blog(LOG_ERROR, "Failed to initialize video:  Graphics module not found");
			return 1;
		}
		case OBS_VIDEO_NOT_SUPPORTED:
		{
			blog(LOG_ERROR, "Failed to initialize video:\n\nRequired graphics API functionality not found.  Your GPU may not be supported.");
			return 1;
		}
		case OBS_VIDEO_INVALID_PARAM:
		{
			blog(LOG_ERROR, "Failed to initialize video:  Invalid parameters");
			return 1;
		}
		default: if (iret != OBS_VIDEO_SUCCESS)
		{
			blog(LOG_ERROR, "Failed to initialize video.  Your GPU may not be supported, or your graphics drivers may need to be updated.");
			return 1;
		}
	}

	// --- load modules
	printf("[==> Load modules\n");
	// if (!load_module("obs-ffmpeg")) return NULL;
	// if (!load_module("obs-outputs")) return NULL;
	// if (!load_module("obs-filters")) return NULL;
	// if (!load_module("obs-transitions")) return NULL;
	// if (!load_module("obs-x264")) return NULL;
	// if (!load_module("rtmp-services")) return NULL;
	// if (!load_module("text-freetype2")) return NULL;
	// if (!load_module("mac-decklink")) return NULL;
	// // if (!load_module("mac-avcapture")) return NULL;
	// // if (!load_module("coreaudio-encoder")) return NULL;
	// if (!load_module("image-source")) return NULL;
	obs_post_load_modules();

	printf("[==> Create scene\n");
  obs_scene_t *scene = obs_scene_create("main scene");
	if (scene == NULL) {
		blog(LOG_ERROR, "failed to create main scene");
		return 1;
	}

	printf("[==> Set output\n");
  obs_set_output_source(0, obs_scene_get_source(scene));

  //// ======== shutdown
	printf("[==> Releasing scene\n");
  obs_scene_release(scene);
	printf("[==> Running obs_shutdown\n");
  obs_shutdown();
  blog(LOG_INFO, "Remained allocs: %ld", bnum_allocs());

  return 0;
}
