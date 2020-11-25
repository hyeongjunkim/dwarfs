/* vim:set ts=2 sw=2 sts=2 et: */
/**
 * \author     Marcus Holland-Moritz (github@mhxnet.de)
 * \copyright  Copyright (c) Marcus Holland-Moritz
 *
 * This file is part of dwarfs.
 *
 * dwarfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dwarfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dwarfs.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <limits>
#include <memory>

#include <folly/Function.h>

namespace dwarfs {

/**
 * A group of worker threads
 *
 * This is an easy to use, multithreaded work dispatcher.
 * You can add jobs at any time and they will be dispatched
 * to the next available worker thread.
 */
class worker_group {
 public:
  using job_t = folly::Function<void()>;

  static struct load_adaptive_tag {
  } load_adaptive;

  /**
   * Create a worker group
   *
   * \param num_workers     Number of worker threads.
   */
  worker_group(const char* group_name = nullptr, size_t num_workers = 1,
               size_t max_queue_len = std::numeric_limits<size_t>::max());

  /**
   * Create a load adaptive worker group
   *
   * \param num_workers     Number of worker threads.
   */
  worker_group(load_adaptive_tag, const char* group_name = nullptr,
               size_t max_num_workers = 1,
               size_t max_queue_len = std::numeric_limits<size_t>::max());

  ~worker_group();

  void stop() { impl_->stop(); }
  void wait() { impl_->wait(); }
  bool running() const { return impl_->running(); }
  bool add_job(job_t&& job) { return impl_->add_job(std::move(job)); }
  size_t queue_size() const { return impl_->queue_size(); }

  class impl {
   public:
    virtual ~impl() = default;

    virtual void stop() = 0;
    virtual void wait() = 0;
    virtual bool running() const = 0;
    virtual bool add_job(job_t&& job) = 0;
    virtual size_t queue_size() const = 0;
  };

 private:
  std::unique_ptr<impl> impl_;
};

} // namespace dwarfs
