/*
 * client-lib.c - linked into gnunet-web client library
 * Copyright (C) 2014  David Barksdale <amatus@amatus.name>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "platform.h"
#include "gnunet_fs_service.h"
#include "gnunet_transport_service.h"
#include "emscripten.h"

int main(int argc, char **argv)
{
  GNUNET_log_setup("client.js", "DEBUG", NULL);
  emscripten_exit_with_live_runtime();
}

void *
GNUNET_FS_start_simple(void *cls)
{
  return GNUNET_FS_start(NULL, "gnunet-web", cls, NULL, 0,
      GNUNET_FS_OPTIONS_END);
}

void *
GNUNET_FS_ProgressInfo_get_publish_cctx(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.publish.cctx;
}

double
GNUNET_FS_ProgressInfo_get_publish_size(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.publish.size;
}

double
GNUNET_FS_ProgressInfo_get_publish_completed(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.publish.completed;
}

const struct GNUNET_FS_Uri *
GNUNET_FS_ProgressInfo_get_publish_completed_chk_uri(
    struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.publish.specifics.completed.chk_uri;
}

void *
GNUNET_FS_ProgressInfo_get_download_cctx(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.download.cctx;
}

double
GNUNET_FS_ProgressInfo_get_download_size(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.download.size;
}

double
GNUNET_FS_ProgressInfo_get_download_completed(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.download.completed;
}

const void *
GNUNET_FS_ProgressInfo_get_download_progress_data(
    struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.download.specifics.progress.data;
}

double
GNUNET_FS_ProgressInfo_get_download_progress_offset(
    struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.download.specifics.progress.offset;
}

double
GNUNET_FS_ProgressInfo_get_download_progress_data_len(
    struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.download.specifics.progress.data_len;
}

unsigned int
GNUNET_FS_ProgressInfo_get_download_progress_depth(
    struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.download.specifics.progress.depth;
}

void *
GNUNET_FS_ProgressInfo_get_search_cctx(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.search.cctx;
}

const struct GNUNET_CONTAINER_MetaData *
GNUNET_FS_ProgressInfo_get_search_result_meta(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.search.specifics.result.meta;
}

const struct GNUNET_FS_Uri *
GNUNET_FS_ProgressInfo_get_search_result_uri(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->value.search.specifics.result.uri;
}

enum GNUNET_FS_Status
GNUNET_FS_ProgressInfo_get_status(struct GNUNET_FS_ProgressInfo *pi)
{
  return pi->status;
}

struct GNUNET_FS_BlockOptions *
GNUNET_FS_BlockOptions_new(double expiration_time, uint32_t anonymity_level,
    uint32_t content_priority, uint32_t replication_level)
{
  struct GNUNET_FS_BlockOptions *bo;

  bo = malloc(sizeof *bo);
  if (bo) {
    bo->expiration_time.abs_value_us = expiration_time;
    bo->anonymity_level = anonymity_level;
    bo->content_priority = content_priority;
    bo->replication_level = replication_level;
  }
  return bo;
}

double
GNUNET_FS_uri_chk_get_file_size2(const struct GNUNET_FS_Uri *uri)
{
  if (GNUNET_FS_uri_test_chk(uri) ||
      GNUNET_FS_uri_test_loc(uri))
    return GNUNET_FS_uri_chk_get_file_size(uri);
  return 0;
}

struct GNUNET_FS_DownloadContext *
GNUNET_FS_download_start_simple(struct GNUNET_FS_Handle *h,
    const struct GNUNET_FS_Uri *uri, uint32_t anonymity, void *cctx)
{
  uint64_t length = GNUNET_FS_uri_chk_get_file_size(uri);

  return GNUNET_FS_download_start(h, uri, NULL, NULL, NULL, 0, length,
      anonymity, 0, cctx, NULL);
}

struct monitor_peers_cls {
  void (*cb)(void *cls,
      const struct GNUNET_PeerIdentity *peer,
      enum GNUNET_TRANSPORT_PeerState state,
      const char *transport_name,
      const void *address,
      size_t address_length);
  void *cb_cls;
};

static void
monitor_peers_callback(void *cls,
    const struct GNUNET_PeerIdentity *peer,
    const struct GNUNET_HELLO_Address *address,
    enum GNUNET_TRANSPORT_PeerState state,
    struct GNUNET_TIME_Absolute state_timeout)
{
  struct monitor_peers_cls *mpc = cls;

  if (!peer) {
    free(cls);
    return;
  }
  if (!address) {
    mpc->cb(mpc->cb_cls, peer, state, NULL, NULL, 0);
    return;
  }
  mpc->cb(mpc->cb_cls, peer, state, address->transport_name, address->address,
      address->address_length);
}

struct GNUNET_TRANSPORT_PeerMonitoringContext *
GNUNET_TRANSPORT_monitor_peers_simple(
    void *peer_callback,
    void *peer_callback_cls)
{
  struct monitor_peers_cls *mpc = malloc(sizeof(struct monitor_peers_cls));

  if (!mpc)
    return NULL;
  mpc->cb = peer_callback;
  mpc->cb_cls = peer_callback_cls;
  return GNUNET_TRANSPORT_monitor_peers(NULL, NULL, GNUNET_NO,
      monitor_peers_callback, mpc);
}

/* vim: set expandtab ts=2 sw=2: */
