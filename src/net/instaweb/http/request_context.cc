/*
 * Copyright 2012 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Author: piatek@google.com (Michael Piatek)

#include "net/instaweb/http/public/request_context.h"

#include "base/logging.h"
#include "net/instaweb/http/public/log_record.h"
#include "net/instaweb/util/public/abstract_mutex.h"
#include "net/instaweb/util/public/request_trace.h"
#include "net/instaweb/util/public/thread_system.h"
#include "net/instaweb/util/public/timer.h"

namespace net_instaweb {

RequestContext::RequestContext(AbstractMutex* logging_mutex, Timer* timer)
    : log_record_(new LogRecord(logging_mutex)),
      using_spdy_(false) {
  timing_info_.Init(timer);
}

RequestContext::RequestContext() : using_spdy_(false) {}

RequestContext::~RequestContext() {
  // Please do not add non-diagnostic functionality here.
  //
  // RequestContexts are reference counted, and doing work in the dtor will
  // result in actions being taken at unpredictable times, leading to difficult
  // to diagnose performance and correctness bugs.
}

RequestContextPtr RequestContext::NewTestRequestContext(
    ThreadSystem* thread_system) {
  return RequestContextPtr(new RequestContext(thread_system->NewMutex(), NULL));
}

AbstractLogRecord* RequestContext::NewSubordinateLogRecord(
    AbstractMutex* logging_mutex) {
  return new LogRecord(logging_mutex);
}

void RequestContext::set_root_trace_context(RequestTrace* x) {
  root_trace_context_.reset(x);
}

AbstractLogRecord* RequestContext::log_record() {
  DCHECK(log_record_.get() != NULL);
  return log_record_.get();
}

void RequestContext::PrepareLogRecordForOutput() {
  log_record()->SetTimingInfo(timing_info_);
}

void RequestContext::WriteBackgroundRewriteLog() {
  if (background_rewrite_log_record_.get() != NULL) {
    background_rewrite_log_record_->WriteLog();
  }
}

AbstractLogRecord* RequestContext::GetBackgroundRewriteLog(
    ThreadSystem* thread_system,
    bool log_urls,
    bool log_url_indices,
    int max_rewrite_info_log_size) {
  // The mutex of the main log record is purposefully used to synchronize the
  // creation of background log record.
  ScopedMutex lock(log_record()->mutex());
  AbstractLogRecord* log_record = background_rewrite_log_record_.get();
  if (log_record == NULL) {
    // We need to create a new log record.
    log_record = NewSubordinateLogRecord(thread_system->NewMutex());
    log_record->SetBackgroundRewriteInfo(log_urls, log_url_indices,
         max_rewrite_info_log_size);
    background_rewrite_log_record_.reset(log_record);
  }
  return log_record;
}

void RequestContext::ReleaseDependentTraceContext(RequestTrace* t) {
  if (t != NULL) {
    delete t;
  }
}

RequestContext::TimingInfo::TimingInfo()
    : timer_(NULL),
      init_ts_ms_(-1),
      start_ts_ms_(-1),
      processing_start_ts_ms_(-1),
      pcache_lookup_start_ts_ms_(-1),
      pcache_lookup_end_ts_ms_(-1),
      parsing_start_ts_ms_(-1),
      fetch_start_ts_ms_(-1),
      fetch_first_byte_ts_ms_(-1),
      fetch_header_ts_ms_(-1),
      fetch_end_ts_ms_(-1),
      end_ts_ms_(-1) {
}

void RequestContext::TimingInfo::Init(Timer* timer) {
  DCHECK(timer_ == NULL);
  timer_ = timer;
  init_ts_ms_ = NowMs();
}

void RequestContext::TimingInfo::RequestStarted() {
  SetToNow(&start_ts_ms_);
  VLOG(2) << "RequestStarted: " << start_ts_ms_;
}

void RequestContext::TimingInfo::FetchStarted() {
  if (fetch_start_ts_ms_ > 0) {
    // It's possible this is called more than once, just ignore subsequent
    // calls.
    return;
  }

  SetToNow(&fetch_start_ts_ms_);
}

int64 RequestContext::TimingInfo::GetElapsedMs() const {
  DCHECK_GE(init_ts_ms_, 0);
  return NowMs() - init_ts_ms_;
}

bool RequestContext::TimingInfo::GetProcessingElapsedMs(
    int64* processing_elapsed_ms) const {
  if (end_ts_ms_ < 0 || start_ts_ms_ < 0) {
    return false;
  }
  int64 elapsed_ms = end_ts_ms_ - start_ts_ms_;
  int64 fetch_elapsed_ms = 0;
  if (GetFetchElapsedMs(&fetch_elapsed_ms)) {
    elapsed_ms -= fetch_elapsed_ms;
  }

  *processing_elapsed_ms = elapsed_ms;
  return true;
}

bool RequestContext::TimingInfo::GetFetchElapsedMs(int64* elapsed_ms) const {
  if (fetch_end_ts_ms_ < 0 || fetch_start_ts_ms_ < 0) {
    return false;
  }

  *elapsed_ms = fetch_end_ts_ms_ - fetch_start_ts_ms_;
  return true;
}

bool RequestContext::TimingInfo::GetTimeToFetchHeaderMs(
    int64* elapsed_ms) const {
  if (fetch_header_ts_ms_ < 0 || fetch_start_ts_ms_< 0) {
    return false;
  }


  const int64 tmp_elapsed_ms = fetch_header_ts_ms_ - fetch_start_ts_ms_;
  if (tmp_elapsed_ms < 0) {
    return false;
  }

  *elapsed_ms = tmp_elapsed_ms;
  return true;
}

int64 RequestContext::TimingInfo::NowMs() const {
  if (timer_ == NULL) {
    return 0;
  }

  return timer_->NowMs();
}

void RequestContext::TimingInfo::SetToNow(int64* ts) const {
  DCHECK_GE(*ts, -1);
  *ts = NowMs();
}

bool RequestContext::TimingInfo::GetTimeFromStart(
    int64 ts_ms, int64* elapsed_ms) const {
  if (ts_ms < 0 || start_ts_ms_ < 0) {
    return false;
  }

  *elapsed_ms = ts_ms - start_ts_ms_;
  return true;
}

}  // namespace net_instaweb