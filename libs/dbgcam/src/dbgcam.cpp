#include <iostream>
#include <spdlog/spdlog.h>

#include <pvcam/master.h>
#include <pvcam/pvcam.h>

rs_bool PV_DECL pl_pvcam_init(void) {
    return PV_OK;
}


rs_bool PV_DECL pl_cam_close(int16 hcam) {
  return PV_OK;
}

rs_bool PV_DECL pl_cam_deregister_callback(int16 hcam, int32 callback_event) {
  return PV_OK;
}

rs_bool PV_DECL pl_cam_open(char* camera_name, int16* hcam, int16 o_mode) {
  return PV_OK;
}

rs_bool PV_DECL pl_cam_register_callback_ex3(int16 hcam, int32 callback_event, void* callback, void* context) {
  return PV_OK;
}

rs_bool PV_DECL pl_pvcam_get_ver(uns16* pvcam_version) {
  *pvcam_version = 0xFF;
  return PV_OK;
}

rs_bool PV_DECL pl_pvcam_uninit(void) {
  return PV_OK;
}

rs_bool PV_DECL pl_create_frame_info_struct(FRAME_INFO** new_frame) {
  *new_frame = new FRAME_INFO{};
  return PV_OK;
}

rs_bool PV_DECL pl_release_frame_info_struct(FRAME_INFO* frame_to_delete) {
  delete frame_to_delete;
  return PV_OK;
}

rs_bool PV_DECL pl_cam_get_total(int16* count) {
  *count = 1;
  return PV_OK;
}

rs_bool PV_DECL pl_cam_get_name(int16 index, char* name) {
  strncpy(name, "TESTCAM", CAM_NAME_LEN);
  return PV_OK;
}

rs_bool PV_DECL pl_get_param(int16 hcam, uns32 param_id, int16 param_attribute, void* param_value) {
  return PV_OK;
}

rs_bool PV_DECL pl_pp_reset(int16 hcam) {
  return PV_OK;
}

rs_bool PV_DECL pl_enum_str_length(int16 hcam, uns32 param_id, uns32 index, uns32* length) {
  return PV_OK;
}

rs_bool PV_DECL pl_get_enum_param(
  int16 hcam,
  uns32 param_id,
  uns32 index,
  int32* value,
  char* desc,
  uns32 length)
{
  return PV_OK;
}

rs_bool PV_DECL pl_set_param(int16 hcam, uns32 param_id, void* param_value) {
  return PV_OK;
}

rs_bool PV_DECL pl_exp_setup_cont(
  int16 hcam,
  uns16 rgn_total,
  const rgn_type* rgn_array,
  int16 exp_mode,
  uns32 exposure_time,
  uns32* exp_bytes,
  int16 buffer_mode)
{
  *exp_bytes = 9680000;
  return PV_OK;
};

rs_bool PV_DECL pl_exp_start_cont(int16 hcam, void* pixel_stream, uns32 size) {
  return PV_OK;
}

int16 pl_error_code() {
  return 0;
}

rs_bool pl_error_message(int16 code, char* errMsg) {
  return PV_OK;
}

rs_bool pl_exp_abort(int16 hcam, int16 cam_state) {
  return true;
}

rs_bool PV_DECL pl_exp_finish_seq(int16 hcam, void* pixel_stream, int16 hbuf) {
  return true;
}

rs_bool PV_DECL pl_exp_get_latest_frame_ex(int16 hcam, void** data, FRAME_INFO* frameInfo) {
    return true;
}

