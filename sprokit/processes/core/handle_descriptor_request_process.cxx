/*ckwg +29
 * Copyright 2017 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "handle_descriptor_request_process.h"

#include <vital/vital_types.h>
#include <vital/types/timestamp.h>
#include <vital/types/timestamp_config.h>
#include <vital/types/image_container.h>
#include <vital/types/object_track_set.h>
#include <vital/types/matrix.h>

#include <vital/algo/handle_descriptor_request.h>

#include <kwiver_type_traits.h>

#include <sprokit/pipeline/process_exception.h>

namespace kwiver
{

namespace algo = vital::algo;

//------------------------------------------------------------------------------
// Private implementation class
class handle_descriptor_request_process::priv
{
public:
  priv();
  ~priv();

  unsigned track_read_delay;

  algo::handle_descriptor_request_sptr m_handler;
}; // end priv class


// =============================================================================

handle_descriptor_request_process
::handle_descriptor_request_process( vital::config_block_sptr const& config )
  : process( config ),
    d( new handle_descriptor_request_process::priv )
{
  // Attach our logger name to process logger
  attach_logger( vital::get_logger( name() ) );

  // Required for negative feedback loop
  set_data_checking_level( check_none );

  make_ports();
  make_config();
}


handle_descriptor_request_process
::~handle_descriptor_request_process()
{
}


// -----------------------------------------------------------------------------
void handle_descriptor_request_process
::_configure()
{
  vital::config_block_sptr algo_config = get_config();

  algo::handle_descriptor_request::set_nested_algo_configuration(
    "handler", algo_config, d->m_handler );

  if( !d->m_handler )
  {
    throw sprokit::invalid_configuration_exception(
      name(), "Unable to create handle_descriptor_request" );
  }

  algo::handle_descriptor_request::get_nested_algo_configuration(
    "handler", algo_config, d->m_handler );

  // Check config so it will give run-time diagnostic of config problems
  if( !algo::handle_descriptor_request::check_nested_algo_configuration(
    "handler", algo_config ) )
  {
    throw sprokit::invalid_configuration_exception(
      name(), "Configuration check failed." );
  }
}


// -----------------------------------------------------------------------------
void
handle_descriptor_request_process
::_step()
{
  // Retrieve inputs from ports
  vital::descriptor_request_sptr request;

  detections = grab_from_port_using_trait( descriptor_request );

  // Get output matrix and detections
  vital::track_descriptor_set_sptr output;

  output = d->m_handler->handle( request );

  // Return all outputs
  push_to_port_using_trait( track_descriptor_set_sptr, output );
}


// -----------------------------------------------------------------------------
void handle_descriptor_request_process
::make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t optional;
  sprokit::process::port_flags_t required;

  required.insert( flag_required );
  required_no_dep.insert( flag_required );

  // -- input --
  declare_input_port_using_trait( descriptor_request, required );

  // -- output --
  declare_output_port_using_trait( track_descriptor_set, optional );
}


// -----------------------------------------------------------------------------
void handle_descriptor_request_process
::make_config()
{
}


// =============================================================================
handle_descriptor_request_process::priv
::priv()
{
}


handle_descriptor_request_process::priv
::~priv()
{
}

} // end namespace
